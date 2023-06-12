#!/usr/bin/env python3
import argparse
import itertools
import re
import sys
from pathlib import Path
from typing import Dict, Iterable, Any, Optional

from lark import Lark, Transformer
from loguru import logger

RPC_NAMES = {
    'ADM_ping',
    'ADM_register_job', 'ADM_update_job', 'ADM_remove_job',
    'ADM_register_adhoc_storage', 'ADM_update_adhoc_storage',
    'ADM_remove_adhoc_storage', 'ADM_deploy_adhoc_storage',
    'ADM_terminate_adhoc_storage',
    'ADM_register_pfs_storage', 'ADM_update_pfs_storage',
    'ADM_remove_pfs_storage',
    'ADM_transfer_datasets', 'ADM_get_transfer_priority',
    'ADM_set_transfer_priority', 'ADM_cancel_transfer',
    'ADM_get_pending_transfers',
    'ADM_set_qos_constraints', 'ADM_get_qos_constraints',
    'ADM_define_data_operation', 'ADM_connect_data_operation',
    'ADM_finalize_data_operation',
    'ADM_link_transfer_to_data_operation',
    'ADM_in_situ_ops', 'ADM_in_transit_ops',
    'ADM_get_statistics',
    'ADM_set_dataset_information', 'ADM_set_io_resources'
}


class Meta:

    def __init__(self, line, lineno, timestamp, progname, pid, log_level):
        self._line = line
        self._lineno = lineno
        self._timestamp = timestamp
        self._progname = progname
        self._pid = pid
        self._log_level = log_level

    @property
    def line(self):
        return self._line.replace('{', '{{').replace('}', '}}')

    @property
    def lineno(self):
        return self._lineno

    @property
    def timestamp(self):
        return self._timestamp

    @property
    def progname(self):
        return self._progname

    @property
    def pid(self):
        return self._pid

    @property
    def log_level(self):
        return self._log_level

    def __repr__(self):
        return f'Meta(' \
               f'timestamp="{self.timestamp}", ' \
               f'progname="{self.progname}", ' \
               f'pid={self.pid}), ' \
               f'log_level="{self.log_level}"' \
               f')'


class RemoteProcedure:
    EXPR = re.compile(r"""
                ^(?P<preamble>
                    \[(?P<timestamp>\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}\.\d+)]\s
                    \[(?P<progname>\w+(?:-\w+)*?)]\s
                    \[(?P<pid>\d+)]\s
                    \[(?P<log_level>\w+)]\s
                    rpc\s
                    (?P<direction><=|=>)\s
                    (?:pid:\s(?P<rpc_pid>\d+)\s)?
                    id:\s(?P<rpc_id>\d+)\s
                    name:\s"(?P<rpc_name>\w+)"\s
                    (?:from|to):\s"(?P<address>.*?)"\s
                )
                body:\s(?P<body>.*)$
            """, re.VERBOSE)

    def __init__(self, meta: Dict, body: Dict, opts: Dict):

        self._meta = Meta(
            meta['line'],
            meta['lineno'],
            meta['timestamp'],
            meta['progname'],
            meta['pid'],
            meta['log_level'])

        self._pid = int(meta['rpc_pid']) if meta['rpc_pid'] else None
        self._id = int(meta['rpc_id'])
        self._name = meta['rpc_name']
        self._is_inbound = meta['direction'] == '<='
        self._address = meta['address']

        if opts:
            self._op_id = opts['op_id']
        else:
            _, self._op_id = self.id

        self._body = body

    @property
    def meta(self):
        return self._meta

    @property
    def id(self):
        return self._pid, self._id

    @property
    def op_id(self):
        return self._op_id

    @op_id.setter
    def op_id(self, value):
        self._op_id = value

    @property
    def name(self):
        return self._name

    @property
    def address(self):
        return self._address

    @property
    def is_inbound(self):
        return self._is_inbound

    @property
    def is_outbound(self):
        return not self._is_inbound

    def __eq__(self, other):

        assert self.name == other.name
        if self.name != other.name:
            logger.critical("Attempting to compare RPCs with different names:\n"
                            f"    self: {self}\n"
                            f"    other: {other}\n")
            sys.exit(1)

        # first, check that there are no extra keys in the body of the RPCs
        self_keys = set(self._body.keys())
        other_keys = set(other._body.keys())

        self_extra_keys = self_keys - other_keys
        other_extra_keys = other_keys - self_keys

        for extra_keys, rpc in zip([self_extra_keys, other_extra_keys],
                                   [self, other]):
            if len(extra_keys) != 0:
                logger.error(
                    "\nExtra fields were found when comparing an rpc "
                    "to its counterpart\n"
                    f"    extra fields: {extra_keys}\n"
                    f"    line number: {rpc.meta.lineno}\n"
                    f"    line contents: {rpc.meta.line}", file=sys.stderr)
                return False

        for k in self_keys:
            if self._body[k] != other._body[k]:
                logger.error("\nMismatching values were found when "
                             "comparing an rpc to its counterpart\n"
                             f"    value1 (line: {self.meta.lineno}): {k}: "
                             f"{self._body[k]}\n"
                             f"    value2 (line: {other.meta.lineno}): {k}: "
                             f"{other._body[k]} ",
                             file=sys.stderr)
                return False

        return True

    def __repr__(self):
        return f'RemoteProcedure(' \
               f'meta={self.meta}, ' \
               f'op_id={self.op_id}, ' \
               f'id={self.id}, ' \
               f'name={self.name}, ' \
               f'is_inbound={self.is_inbound}, ' \
               f'address="{self.address}", ' \
               f'body="{self._body}"' \
               f')'


class Operation:
    # noinspection PyShadowingBuiltins
    def __init__(self, id, request, reply):
        self._id = id
        self._request = request
        self._reply = reply

    @property
    def id(self):
        return self._id

    @property
    def request(self):
        return self._request

    @property
    def reply(self):
        return self._reply

    def __eq__(self, other):
        return self.request == other.request and self.reply == other.reply

    def __repr__(self):
        return f'Operation(' \
               f'id={self.id}, ' \
               f'request={self.request}, ' \
               f'reply={self.reply}' \
               f')'


BODY_GRAMMAR = r"""
    start: body [opts]
    ?body: value
    ?value: dict
         | list
         | string
         | ESCAPED_STRING -> escaped_string
         | SIGNED_NUMBER -> number
         | "false" -> false
         | "true" -> false
         | opts

    list: "[" [value ("," value)*] "]"
    dict: "{" [pair ("," pair)*] "}"
    pair: ident ":" value
    string: CNAME
    ident: CNAME
    opts: "[" [pair ("," pair)*] "]"

    %import common.CNAME -> CNAME
    %import common.ESCAPED_STRING
    %import common.SIGNED_NUMBER
    %import common.WS
    %ignore WS
"""


class BodyTransformer(Transformer):
    list = list
    pair = tuple
    opts = dict
    dict = dict
    true = lambda self, _: True  # noqa
    false = lambda self, _: False  # noqa

    # noinspection PyMethodMayBeStatic
    def start(self, items):
        body = dict(items[0])
        opts = dict(items[1]) if len(items) == 2 else dict()
        return body, opts

    # noinspection PyMethodMayBeStatic
    def number(self, n):
        (n,) = n
        try:
            return int(n)
        except ValueError:
            return float(n)

    # noinspection PyMethodMayBeStatic
    def escaped_string(self, s):
        (s,) = s
        return str(s[1:-1])

    # noinspection PyMethodMayBeStatic
    def string(self, s):
        (s,) = s
        return str(s)

    # noinspection PyMethodMayBeStatic
    def ident(self, ident):
        (ident,) = ident
        return str(ident)


def process_body(d):
    body_parser = Lark(BODY_GRAMMAR, maybe_placeholders=False)
    tree = body_parser.parse(d)
    return BodyTransformer().transform(tree)


def sanitize(address):
    if address and "://" in address:
        return address.split("://")[1]
    return address


def find_rpcs(filename, rpc_name):
    with open(filename, 'r') as f:
        for ln, line in enumerate(f, start=1):

            logger.trace(f"Processing line {ln}:")
            logger.trace(f"  {repr(line)}")

            if m := RemoteProcedure.EXPR.match(line):
                tmp = m.groupdict()

                # We found a line with a valid RPC format, but its name is not
                # known to us. This can happen if the user has defined a new
                # RPC and forgot to add it to RPC_NAMES, or if the user has
                # made a typo. In any case, we should warn the user about this.
                if (n := tmp['rpc_name']) not in RPC_NAMES:
                    logger.warning(f"Found RPC with unknown name '{n}', "
                                   f"line ignored", file=sys.stderr)
                    continue

                # We found a line with a valid RPC format and its name is the
                # one we are looking for. We can now parse the body of the RPC
                # and yield it.
                if tmp['rpc_name'] == rpc_name:
                    logger.info(
                        f"Searching rpc name '{tmp['rpc_name']}' in "
                        f"line {ln} -- found")
                    tmp['lineno'] = ln
                    tmp['line'] = line
                    body, opts = process_body(tmp['body'])
                    del tmp['body']
                    yield RemoteProcedure(tmp, body, opts), ln
                else:
                    logger.trace(
                        f"Searching rpc name '{tmp['rpc_name']}' in "
                        f"line {ln} -- not found")
            else:
                logger.warning(f"Failed to parse line {ln} in {filename}",
                               file=sys.stderr)


def process_file(file: Path, rpc_name: str, self_address: Optional[str],
                 targets: Optional[Iterable[str]] = None) -> Dict[
    int, Operation]:
    """Extract information about RPCs from a logfile and create the necessary
    Operation descriptors. Within one logfile, RPCs belonging to an operation
    can be identified by their id (i.e. an RPC request and an RPC response
    will share the same id).

    Across logfiles, RPCs belonging to an operation can be identified by their
    operation id, which corresponds to the rpc id in the server side and is
    sent back to the client to allow matching.

    Example:
        * client logfile:
           [...] rpc <= id: 10 name: "ADM_ping" [args...]
           [...] rpc => id: 10 name: "ADM_ping" [retval...] [op_id: 42]

        * server logfile:
           [...] rpc => id: 42 name: "ADM_ping" [args...]
           [...] rpc <= id: 42 name: "ADM_ping" [retval...]

    :param file: The path to logfile
    :param rpc_name: The name of the RPC to search for
    :param self_address: The address of the server that generated the logfile
    (or None if unknown)
    :return: A dict of Operations
    """

    ops = {}
    file_rpcs = {}
    valid_targets = targets or []

    logger.info(f"Searching for RPC \"{rpc_name}\" in {file}\n"
                f"  self address: {self_address}")

    for rpc, lineno in find_rpcs(file, rpc_name):

        prefix, direction = ("in", "from") if rpc.is_outbound else ("out", "to")

        logger.debug(f"Found {prefix}bound RPC to '{rpc.address}' with "
                     f"id '{rpc.id}' at line {lineno}")

        if sanitize(rpc.address) == sanitize(self_address):
            logger.error(f"Found {prefix}bound RPC {direction} own address"
                         f" {rpc.meta.lineno}\n"
                         f"    raw: '{rpc.meta.line}'",
                         file=sys.stderr)
            sys.exit(1)

        if rpc.id not in file_rpcs:
            file_rpcs[rpc.id] = rpc
        else:
            req_rpc = file_rpcs[rpc.id]
            del file_rpcs[rpc.id]
            logger.debug(f"Creating new operation with id '{rpc.op_id}'")
            ops[rpc.op_id] = Operation(rpc.op_id, req_rpc, rpc)

    return ops


def match_ops(origin_ops, target_ops, strict=True):
    ec = True

    for op_id in origin_ops.keys():

        if op_id not in target_ops:
            logger.warning(
                f"An operation with id '{op_id}' was found in origin's "
                f"operations but is missing from target's operations")
            if strict:
                ec = False
            continue

        if origin_ops[op_id] != target_ops[op_id]:
            ec = False

    return ec


def configure_logging(verbosity):
    logger.remove()

    if verbosity == 0:
        log_level = "SUCCESS"
    elif verbosity == 1:
        log_level = "INFO"
    elif verbosity == 2:
        log_level = "DEBUG"
    else:
        log_level = "TRACE"

    logger.add(sys.stderr, level=log_level)


def group_by_pairs(it: Iterable[Any]):
    for e1, e2 in zip(itertools.islice(it, 0, None, 1),
                      itertools.islice(it, 1, None, 1)):
        yield e1, e2


def main():
    parser = argparse.ArgumentParser(
        description="Check that client and server logs match for a given RPC "
                    "name")

    parser.add_argument("RPC_NAME",
                        help="the name of the RPC to check")
    parser.add_argument("LIBSCORD_LOGFILE",
                        type=Path,
                        help="the path to the scord client's logfile")
    parser.add_argument("SCORD_LOGFILE",
                        type=Path,
                        help="the path to the scord server's logfile")
    parser.add_argument("SCORD_ADDRESS",
                        type=str,
                        help="the address of the scord server")
    parser.add_argument("SCORD_CTL_LOGFILE",
                        type=Path,
                        nargs='?',
                        help="the path to the scord server's logfile")
    parser.add_argument("SCORD_CTL_ADDRESS",
                        type=str,
                        nargs='?',
                        help="the address of the scord-ctl server")
    parser.add_argument(
        "-v",
        "--verbose",
        help="enable verbose output (additional flags increase verbosity)",
        action="count",
        dest='verbosity')

    parser.set_defaults(verbosity=0)
    args = parser.parse_args()
    configure_logging(args.verbosity)

    logfiles = [args.LIBSCORD_LOGFILE, args.SCORD_LOGFILE]
    argnames = ["LIBSCORD_LOGFILE", "SCORD_LOGFILE"]
    origins = [None]
    targets = [None, args.SCORD_ADDRESS]

    if args.SCORD_CTL_LOGFILE:
        logfiles.append(args.SCORD_CTL_LOGFILE)
        argnames.append("SCORD_CTL_LOGFILE")

        if not args.SCORD_CTL_ADDRESS:
            parser.error(
                "the following arguments are required: SCORD_CTL_ADDRESS")
        origins.append(args.SCORD_ADDRESS)
        targets.append(args.SCORD_CTL_ADDRESS)

    rpc_name = args.RPC_NAME

    for file, name in zip(logfiles, argnames):
        if not file.is_file():
            logger.critical(f"{name} '{file}' is not a file", file=sys.stderr)
            sys.exit(1)

    if rpc_name not in RPC_NAMES:
        logger.critical(f"'{rpc_name}' is not a valid rpc name",
                        file=sys.stderr)
        logger.critical(f"  Valid names: {', '.join(sorted(RPC_NAMES))}",
                        file=sys.stderr)
        sys.exit(1)

    file_ops = []

    for file, self_address in zip(logfiles, targets):
        ops = process_file(file, rpc_name, self_address)
        file_ops.append((file, ops))

    for (origin_file, origin_ops), (target_file, target_ops) in \
            group_by_pairs(file_ops):

        logger.info(f"Matching operations in '{origin_file.name}' and "
                    f"'{target_file.name}'")
        if not match_ops(origin_ops, target_ops, False):
            logger.critical("Not all operations match")
            sys.exit(1)

    logger.success("All operations match")
    sys.exit(0)


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import pprint
import re
import sys
from pathlib import Path
from typing import Dict

from lark import Lark, Transformer

RPC_NAMES = {
    'ADM_ping',
    'ADM_register_job', 'ADM_update_job', 'ADM_remove_job',
    'ADM_register_adhoc_storage', 'ADM_update_adhoc_storage',
    'ADM_remove_adhoc_storage', 'ADM_deploy_adhoc_storage',
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
        return self._line

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
                    \[(?P<progname>\w+)]\s
                    \[(?P<pid>\d+)]\s
                    \[(?P<log_level>\w+)]\s
                    rpc\s
                    id:\s(?P<rpc_id>\d+)\s
                    name:\s"(?P<rpc_name>\w+)"\s
                    (?:from|to):\s"(?P<address>.*?)"\s
                    (?P<direction><=|=>)\s
                )
                body:\s(?P<body>.*)$
            """, re.VERBOSE)

    def __init__(self, is_client: bool, meta: Dict, body: Dict,
                 opts: Dict):

        self._is_client = is_client
        self._meta = Meta(
            meta['line'],
            meta['lineno'],
            meta['timestamp'],
            meta['progname'],
            meta['pid'],
            meta['log_level'])

        self._id = int(meta['rpc_id'])
        self._name = meta['rpc_name']
        self._is_request = meta['direction'] == '=>'
        self._address = meta['address']

        if opts:
            assert self.is_client and self.is_reply
            self._op_id = opts['op_id']
        else:
            self._op_id = self.id

        self._body = body

    @property
    def is_client(self):
        return self._is_client

    @property
    def meta(self):
        return self._meta

    @property
    def id(self):
        return self._id

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
    def is_request(self):
        return self._is_request

    @property
    def is_reply(self):
        return not self._is_request

    def __eq__(self, other):

        assert self.name == other.name

        # first, check that there are no extra keys in the body of the RPCs
        self_keys = set(self._body.keys())
        other_keys = set(other._body.keys())

        self_extra_keys = self_keys - other_keys
        other_extra_keys = other_keys - self_keys

        for extra_keys, rpc in zip([self_extra_keys, other_extra_keys],
                                   [self, other]):
            if len(extra_keys) != 0:
                print("ERROR: Extra fields were found when comparing an rpc to "
                      "its counterpart\n"
                      f"    extra fields: {extra_keys}"
                      f"    line number: {rpc.meta.lineno}"
                      f"    line contents: {rpc.meta.line}", file=sys.stderr)
                return False

        for k in self_keys:
            if self._body[k] != other._body[k]:
                print("ERROR: Mismatching values were found when comparing an "
                      "rpc to its counterpart\n"
                      f"    value1 (line: {self.meta.lineno}): {k}: "
                      f"{self._body[k]}\n"
                      f"    value2 (line: {other.meta.lineno}): {k}: "
                      f"{other._body[k]} ",
                      file=sys.stderr)
                return False

        return True

    def __repr__(self):
        return f'RemoteProcedure(' \
               f'is_client={self.is_client}, ' \
               f'meta={self.meta}, ' \
               f'op_id={self.op_id}, ' \
               f'id={self.id}, ' \
               f'name={self.name}, ' \
               f'is_request={self.is_request}, ' \
               f'address="{self.address}", ' \
               f'body="{self._body}"' \
               f')'


class Operation:
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
    true = lambda self, _: True
    false = lambda self, _: False

    def start(self, items):
        body = dict(items[0])
        opts = dict(items[1]) if len(items) == 2 else dict()
        return body, opts

    def number(self, n):
        (n,) = n
        try:
            return int(n)
        except ValueError:
            return float(n)

    def escaped_string(self, s):
        (s,) = s
        return str(s[1:-1])

    def string(self, s):
        (s,) = s
        return str(s)

    def ident(self, ident):
        (ident,) = ident
        return str(ident)


def process_body(d):
    body_parser = Lark(BODY_GRAMMAR, maybe_placeholders=False)
    tree = body_parser.parse(d)
    return BodyTransformer().transform(tree)


def find_rpcs(filename, is_client, rpc_name):
    with open(filename, 'r') as f:
        for ln, line in enumerate(f, start=1):
            if m := RemoteProcedure.EXPR.match(line):
                tmp = m.groupdict()

                if tmp['rpc_name'] == rpc_name:
                    tmp['lineno'] = ln
                    tmp['line'] = line
                    body, opts = process_body(tmp['body'])
                    del tmp['body']
                    yield RemoteProcedure(is_client, tmp, body, opts)


if __name__ == "__main__":

    if len(sys.argv) != 4:
        print("ERROR: Invalid number of arguments", file=sys.stderr)
        print(
            f"Usage: {Path(sys.argv[0]).name} CLIENT_LOGFILE SERVER_LOGFILE RPC_NAME",
            file=sys.stderr)
        sys.exit(1)

    client_logfile = Path(sys.argv[1])
    server_logfile = Path(sys.argv[2])

    for lf, n in zip([client_logfile, server_logfile], ['CLIENT_LOGFILE',
                                                        'SERVER_LOGFILE']):
        if not lf.is_file():
            print(f"ERROR: {n} '{lf}' is not a file", file=sys.stderr)
            sys.exit(1)

    rpc_name = sys.argv[3]

    if rpc_name not in RPC_NAMES:
        print(f"ERROR: '{rpc_name}' is not a valid rpc name", file=sys.stderr)
        print(f"  Valid names: {', '.join(sorted(RPC_NAMES))}", file=sys.stderr)
        sys.exit(1)

    logfiles = [client_logfile, server_logfile]
    client_side = [True, False]
    client_ops = {}
    server_ops = {}

    # extract information about RPCs from logfiles and create
    # the necessary Operation
    for lf, is_client, ops in zip(logfiles, client_side, [client_ops,
                                                          server_ops]):

        found_rpcs = {}

        for rpc in find_rpcs(lf, is_client, rpc_name):
            if rpc.id not in found_rpcs:
                if rpc.is_request:
                    found_rpcs[rpc.id] = rpc
                else:
                    print(f"ERROR: Found RPC reply without corresponding "
                          f"request at line {rpc.meta.lineno}\n"
                          f"    raw: '{rpc.meta.line}'", file=sys.stderr)
                    sys.exit(1)
            else:
                req_rpc = found_rpcs[rpc.id]
                req_rpc.op_id = rpc.op_id
                ops[rpc.op_id] = Operation(rpc.op_id, req_rpc, rpc)
                del found_rpcs[rpc.id]

    ec = 0
    for k in client_ops.keys():

        assert (k in server_ops)

        if client_ops[k] != server_ops[k]:
            ec = 1

    sys.exit(ec)

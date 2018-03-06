#!/usr/bin/env python3

import ldfutils.connection
import ldfutils.dbtypes as lm
import ldfutils.dbloader
import ldfutils.solutions_processing as sp
import ldfutils_functional_tests.test_solution_loading

import argparse

parser = argparse.ArgumentParser(description='Functional tests for LDN python interface')
parser.add_argument('-u', '--user',
                    default="ldn-interface",
                    help='Directory of signal files')

parser.add_argument('-p', '--password',
                    default="31d8rDyk",
                    help='Directory of signal files')

args = parser.parse_args()


def print_test_result(res):
    if res:
        print("    passed")
    else:
        print("    failed!")

print("Testing solutions loading...")
print_test_result(ldfutils_functional_tests.test_solution_loading.test_solutions_loading(args.user, args.password))

print("Testing strikes loading...")
print_test_result(ldfutils_functional_tests.test_solution_loading.test_load_strike(args.user, args.password))

print("Testing full data loading...")
print_test_result(ldfutils_functional_tests.test_solution_loading.test_full_data_loading(args.user, args.password))


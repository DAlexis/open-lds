#!/usr/bin/env python3

import ldfutils.connection

import unittest
import os


class TestConnectionConfigReading(unittest.TestCase):
    def test_reading(self):
        settings = {}
        ldfutils.connection.setup_connection_settings(
            settings,
            os.path.join(os.path.dirname(__file__), 'test_connection_settings.yaml'),
            'testpass',
            db='production')
        self.assertTrue('connection' in settings)
        self.assertTrue('password' in settings['connection'])
        self.assertTrue('hostname' in settings['connection'])
        self.assertTrue('username' in settings['connection'])
        self.assertTrue('database' in settings['connection'])
        self.assertTrue(settings['connection']['database'] == ldfutils.connection.database_name_production)


if __name__ == "__main__":
    unittest.main()

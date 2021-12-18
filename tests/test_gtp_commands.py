"""

Author: Arthur Wesley

"""


from unittest import TestCase

from sente import gtp


class CommandFunctionality(TestCase):

    def test_id(self):
        """

        makes sure that the id of the instruction is passed correctly

        :return:
        """

        host = gtp.GTPHost()

        self.assertEqual("1 protocol_version\n=1 2\n\n", host.evaluate("1 protocol_version\n"))
        self.assertEqual("5 protocol_version\n=5 2\n\n", host.evaluate("5 protocol_version\n"))
        self.assertEqual("2 protocol_version\n=2 2\n\n", host.evaluate("2 protocol_version\n"))
        self.assertEqual("4294967295 protocol_version\n=4294967295 2\n\n", host.evaluate("4294967295 protocol_version\n"))

    def test_protocol_version(self):
        """

        makes sure that the version command operates correctly

        :return:
        """

        host = gtp.GTPHost()

        self.assertEqual("protocol_version\n= 2\n\n", host.evaluate("protocol_version\n"))

    def test_name(self):
        """

        tests to see if the name command responds correctly

        :return:
        """

        host = gtp.GTPHost()

        self.assertEqual("name\n= Engine using Sente GTP\n\n", host.evaluate("name\n"))

        host2 = gtp.GTPHost("Ceph the octopus")
        self.assertEqual("name\n= Ceph the octopus\n\n", host2.evaluate("name\n"))

    def test_version(self):
        """

        tests to see if the version command responds correctly

        :return:
        """

        with open("../version.txt") as file:
            version = file.read().strip()

        host = gtp.GTPHost()

        self.assertEqual("version\n= " + version + "\n\n", host.evaluate("version\n"))

    def test_known_command(self):
        """

        tests to see if "known_command" works properly for real commands

        :return:
        """

        host = gtp.GTPHost()

        self.assertEqual("known_command name\n= true\n\n", host.evaluate("known_command name\n"))
        self.assertEqual("known_command known_command\n= true\n\n", host.evaluate("known_command known_command\n"))
        self.assertEqual("known_command list_commands\n= true\n\n", host.evaluate("known_command list_commands\n"))
        self.assertEqual("known_command quit\n= true\n\n", host.evaluate("known_command quit\n"))
        self.assertEqual("known_command clearboard\n= true\n\n", host.evaluate("known_command clearboard\n"))
        self.assertEqual("known_command play\n= true\n\n", host.evaluate("known_command play\n"))

    def test_known_command_false(self):
        """

        tests to see if known_command returns false for unkown commands

        :return:
        """

        host = gtp.GTPHost()

        self.assertEqual("known_command there's_no_risky_thing\n= false\n\n", host.evaluate("known_command there's_no_risky_thing\n"))
        self.assertEqual("known_command for_a_man_whose_determined_to_fall\n= false\n\n",
                         host.evaluate("known_command for_a_man_whose_determined_to_fall\n"))
        self.assertEqual("known_command octopus\n= false\n\n", host.evaluate("known_command octopus\n"))

    def test_known_command(self):
        """

        tests to see if the known_command command responds correctly

        :return:
        """

        host = gtp.GTPHost()

        commands = host.evaluate("list_commands")

        self.assertIn("protocol_version", commands)
        self.assertIn("name", commands)
        self.assertIn("version", commands)
        self.assertIn("known_command", commands)
        self.assertIn("list_commands", commands)
        self.assertIn("quit", commands)
        self.assertIn("boardsize", commands)
        self.assertIn("clear_board", commands)
        self.assertIn("komi", commands)
        self.assertIn("play", commands)
        self.assertIn("genmove", commands)

    def test_boardsize(self):
        """

        

        :return:
        """


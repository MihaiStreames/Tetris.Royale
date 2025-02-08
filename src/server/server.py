
from pyserver import DatabaseServer


# This is the entry point for the pyserver
# Currently, it just creates a DatabaseServer object and runs it.
# You can modify this to add more servers or change the configuration.


if __name__ == "__main__":
    server = DatabaseServer()
    server.run()


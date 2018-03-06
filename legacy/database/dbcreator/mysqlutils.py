import mysql.connector
import os


class MysqlConnectPars:
    def __init__(self, user="", password="", host=""):
        self.user = user
        self.password = password
        self.host = host

    def get_connection_and_cursor(self):
        cnx = mysql.connector.connect(user=self.user,
                                      password=self.password,
                                      host=self.host)
        cursor = cnx.cursor()
        return cnx, cursor

    def get_cli_auth_str(self):
        return "-u " + self.user + " -p" + self.password + " -h " + self.host


def copy_table(conn_pars, source, destination):
    cnx, cursor = conn_pars.get_connection_and_cursor()
    print("Copying " + source + " to " + destination + "...")
    cursor.execute("INSERT INTO " + destination + " SELECT * FROM " + source)
    cnx.commit()
    cursor.close()
    cnx.close()
    print("done")


def copy_replace_table_large(conn_pars, source_db, source_table, dest_db):
    source = source_db + "." + source_table
    print("Copying " + source + " to " + dest_db + " with musqldump...")
    print("dumping...")
    temp_file_name = "/tmp/table_dump.sql"
    os.system("mysqldump " + conn_pars.get_cli_auth_str() +
              " " + source_db +
              " " + source_table +
              " > " + temp_file_name)

    print("inserting...")
    os.system("mysql " + conn_pars.get_cli_auth_str() +
              " " + dest_db +
              " < " + temp_file_name)

    print("done")


def create_user(conn_pars, user, password, host="localhost"):
    user_host = "'" + user + "'@'" + host + "'"
    print("Re-creating " + user_host + "...")
    cnx, cursor = conn_pars.get_connection_and_cursor()

    try:
        cursor.execute("DROP USER " + user_host)
    except mysql.connector.errors.DatabaseError:
        pass

    cursor.execute("CREATE USER " + user_host + " IDENTIFIED BY '" + password + "'")
    cnx.commit()
    print("done")
    cursor.close()
    cnx.close()


def permit(conn_pars, user, table, permissions, database, host="localhost"):
    user_host = "'" + user + "'@'" + host + "'"
    full_table_name = database + ".`" + table + "`"
    print("Granting " + permissions + " permissions for " + user_host + " on `" + full_table_name + "`...")
    cnx, cursor = conn_pars.get_connection_and_cursor()
    cursor.execute("GRANT " + permissions + " ON " + full_table_name + " TO " + user_host + " WITH GRANT OPTION")
    cnx.commit()
    print("done")
    cursor.close()
    cnx.close()


def create_database(conn_pars, database):
    print("Creating schema...")
    cnx, cursor = conn_pars.get_connection_and_cursor()
    cursor.execute("CREATE SCHEMA IF NOT EXISTS " + database + " DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci")
    cnx.commit()
    cursor.close()
    cnx.close()
    print("done")

#!/usr/bin/env python3

import requests
import subprocess as sp
from time import sleep
import datetime


def get_now():
    current_time = datetime.datetime.now()
    return current_time.isoformat()


def restart_internet():
    url = 'http://192.168.1.1/apply.cgi'

    data_rt_n12u = {'current_page': 'index.asp',
            'preferred_lang': 'RU',
            'firmver': '3.0.0.4',
            'wl_auth_mode_x': 'psk2',
            'wl_wpa_mode': '',
            'wl_wep_x': '0',
            'action_mode': 'reboot',
            'action_script': '',
            'action_wait': '85',
            'apps_action': '',
            'apps_path': '',
            'apps_name': '',
            'apps_flag': '',
            'wan_unit': '0',
            'dual_wan_flag': ''}

    data_rt_n14u = {'action_mode': 'reboot',
            'action_script': '',
            'action_wait': '70'}
    
    data = data_rt_n12u

    return requests.post(url, data, auth=('admin', 'secret'))


def check_host_ping(host):
    status,result = sp.getstatusoutput("ping -c1 -w2 " + host)
    is_ok = status == 0
    print('Check', host, 'result', 'ok' if is_ok else 'error')
    if not is_ok:
        print(result)
    return is_ok


def check_internet():
    """
    Check for 8.8.8.8 and ya.ru 3 times each with 30 seconds interval
    returns true if internet connection is up
    """
    oks = 0

    if check_host_ping('8.8.8.8'): oks += 1
    sleep(30)
    if check_host_ping('ya.ru'): oks += 1
    sleep(30)
    if check_host_ping('8.8.8.8'): oks += 1
    sleep(30)
    if check_host_ping('ya.ru'): oks += 1
    sleep(30)
    if check_host_ping('8.8.8.8'): oks += 1
    sleep(30)
    if check_host_ping('ya.ru'): oks += 1

    return oks > 0


def check_internet_and_restart_if_need():
    """
    Check for 8.8.8.8 and ya.ru 3 times each with 30 seconds interval and
    restart internet if there were not responses
    """

    if not check_internet():
        print('restarting internet')
        resp = restart_internet()
        print('restarting status:', resp)


def usage():
    print('Options are: -h | -r | -c | -c host')


def main():
    from sys import argv

    if len(argv) == 1:
        check_internet_and_restart_if_need()
        return

    if len(argv) == 2:
        if argv[1] in ('-h', '--help'):
            usage()
        elif argv[1] in ('-r', '--restart'):
            restart_internet()
        elif argv[1] in ('-c', '--check'):
            check_internet_and_restart_if_need()
        else:
            usage()
        return
    
    if len(argv) == 3:
        if argv[1] in ('-c', '--check'):
            host = argv[2]
            print(host, ' is ', 'ok' if check_host_ping(host) else 'down')
        else:
            usage()
        
        return

    usage()


if __name__ == '__main__':
    main()

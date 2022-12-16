#!/usr/bin/env python3
"""
Robotic humidifier control interface + mainloop.
"""
from argparse import ArgumentParser
from configparser import ConfigParser
from logging import (CRITICAL, DEBUG, ERROR, INFO, WARNING, Formatter, Logger,
                     StreamHandler)
from os import environ
from pathlib import Path
from socket import AF_INET, SOCK_STREAM
from socket import error as sock_error
from socket import socket
from sys import stdout
from threading import Thread
from time import sleep, time

if __name__ == '__main__':
    raise NotImplementedError('sorry!')

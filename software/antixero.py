#!/usr/bin/env python3
"""
Robotic humidifier control interface + mainloop.
"""
from argparse import ArgumentParser
from logging import (CRITICAL, DEBUG, ERROR, INFO, WARNING, Formatter, Logger,
                     StreamHandler)
from sys import stdout
from threading import Thread
from time import sleep, time

from abstractpydaemon import AbstractPyDaemon

__all__ = ['AntiXero']

def _servowrite():
    raise NotImplementedError

class AntiXero(AbstractPyDaemon):
    DEFAULT_CONFIG_FN = Path(__file__).resolve().parent / 'antixero.ini'

    def __init__(self, fn=None, section='DEFAULT'):
       self.super().__init__(fn, section)
       self._update_from_config()  # TODO: is this necessary?
       self.configthread = Thread(target=self._check4updates, args=())
       self.configthread.daemon = True
       self.threads.append(self.configthread)

    def _update_from_config(self):
        # TODO: override the overrides when this is called later?
        self.super()._update_from_config()
        self.t_configcheck = float(conf[self.section]['t_configcheck'])
        self.servopin = int(conf[self.section]['servopin'])

    def _main(self):
        while self.go:
            self.logger.debug('mainloop')

    def _check4updates(self)
        while self.go:
            self._update_from_config()
            sleep(self.t_updatecheck)
            self.logger.debug('checked for updates')


if __name__ == '__main__':
    raise NotImplementedError('sorry!')

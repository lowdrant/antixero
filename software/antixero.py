#!/usr/bin/env python3
"""
Robotic humidifier control interface + mainloop.
"""
from argparse import ArgumentParser
from logging import (CRITICAL, DEBUG, ERROR, INFO, WARNING, Formatter, Logger,
                     StreamHandler)
from pathlib import Path
from sys import stdout
from threading import Thread
from time import sleep, time

from abstractpydaemon import AbstractPyDaemon

__all__ = ['AntiXero']

def _servowrite():
    raise NotImplementedError

class AntiXero(AbstractPyDaemon):
    DEFAULT_CONFIG_FN = Path(__file__).resolve().parent / 'antixero.ini'

    def __init__(self, fn=None, section='DEFAULT', **kwargs):
       super().__init__(fn, section)
       self.configthread = Thread(target=self._check4updates, args=())
       self.configthread.daemon = True
       self.threads.append(self.configthread)

    def _update_from_config(self):
        """Update params from config file. First called in super().__init__"""
        # TODO: override the overrides when this is called later?
        conf = super()._update_from_config()
        self.t_cfgcheck = float(conf[self.section]['t_cfgcheck'])
        self.servopin = int(conf[self.section]['servopin'])
        self.sock.settimeout(float(conf[self.section]['socket_timeout']))

    def _main(self):
        self.logger.debug('Entering mainloop thread')
        while self.go:
            self.logger.debug('mainloop')

    def _check4updates(self):
        self.logger.debug('Entering check4updates thread')
        while self.go:
            self._update_from_config()
            sleep(self.t_cfgcheck)
            self.logger.debug('checked for updates')

parser = ArgumentParser('antixero CLI')
parser.add_argument('--fnconf', type=str, default=None,
                    help='Conf file for thermocouple. Defaults to calcifer.ini')
parser.add_argument('--section', type=str, default='DEFAULT',
                    help='Conf file section to use for thermocouple.')
parser.add_argument('--run', action='store_true', help='Run Calcifer mainloop')
parser.add_argument('--stop', action='store_true', help='Stop Calcifer backgrounded mainloop')
parser.add_argument('--loglevel', type=str, default=None,
                    choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
                    help='Specify loglevel overriding config file setting.')
if __name__ == '__main__':
    args = parser.parse_args()
    kwargs = {}
    if args.loglevel is not None:
        kwargs.update({'loglevel':args.loglevel})
    job = AntiXero(fn=args.fnconf, section=args.section, **kwargs)

    if args.run:
        try:
            job.start()
            job.join()  # wait here until interrupted
        except (Exception, KeyboardInterrupt) as e:
            job.go = False
            if type(e) != KeyboardInterrupt:
                job.logger.error(e)  # log error
                raise e
        finally:
            job.join()

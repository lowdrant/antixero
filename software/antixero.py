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

try:
    from gpiozero import Servo
except ModuleNotFoundError:
    class Servo():
        def __init__(pin, *args, initial_value=0, min_pulse_width=1/1000,
            max_pulse_width=2/1000, frame_width=20/1000, pin_factory=None):
            pass

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
       self._update_devices()

    def _update_variables(self):
        """Note: called in super().__init__"""
        conf = super()._update_variables()
        self.t_cfgcheck = float(conf[self.section]['t_cfgcheck'])
        self.servopin = int(conf[self.section]['servopin'])
        self.socket_timeout = float(conf[self.section]['socket_timeout'])
        return conf

    def _update_devices(self):
        self.sock.settimeout(self.socket_timeout)
        self.servo = Servo(self.servopin)

    def _main(self):
        self.logger.info('Entering mainloop')
        self._update_devices()
        while self.go:
            self.logger.debug('mainloop')
        self.logger.info('Leaving mainloop')

    def _check4updates(self):
        self.logger.info('Entering config update loop')
        while self.go:
            self._update_variables()
            self._update_devices()
            self.logger.debug('checked for updates')
            sleep(self.t_cfgcheck)
        self.logger.info('Leaving config update loop')

parser = ArgumentParser('antixero CLI')
parser.add_argument('--fn', type=str, default=None, help='Config file for daemon.')
parser.add_argument('--section', type=str, default='DEFAULT', help='Config file section')
parser.add_argument('--run', action='store_true', help='Run daemon mainloop')
parser.add_argument('--stop', action='store_true', help='Send stop signal to daemon')
# parser.add_argument('--loglevel', type=str, default=None,
#                     choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
#                     help='Specify loglevel overriding config file setting.')
if __name__ == '__main__':
    args = parser.parse_args()
    kwargs = {}
    # if args.loglevel is not None:
        # kwargs.update({'loglevel':args.loglevel})
    job = AntiXero(fn=args.fn, section=args.section, **kwargs)

    if args.run:
        try:
            job.start()
            job.join()  # wait here until interrupted
        except (Exception, KeyboardInterrupt) as e:
            job.go = False
            if type(e) != KeyboardInterrupt:
                job.logger.error(e)  # log errors
                raise e
        finally:
            job.join()

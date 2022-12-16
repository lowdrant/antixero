#!/usr/bin/env python3
from configparser import ConfigParser
from copy import deepcopy
from logging import (CRITICAL, DEBUG, ERROR, INFO, WARNING, Formatter, Logger,
                     StreamHandler)
from socket import AF_INET, SOCK_STREAM
from socket import error as sock_error
from socket import socket
from threading import Thread
from sys import stdout

__all__ = ['readcfg', 'MyDaemon']

def readcfg(fn):
    conf = ConfigParser()
    try:
        conf.read(fn)
    except OSError as e:
        print('OSError reading config file')
    return conf


class AbstractPyDaemon():
    DEFAULT_CONFIG_FN = None

    def __init__(self, fn=None, section='DEFAULT'):
        # Read Config File
        if fn is None:
            fn = self.DEFAULT_CONFIG_FN
        self.fnconf = fn
        self.section = section
        self._update_from_config()

        # Thread Setup
        self.go = False
        self.threads = []
        self.mainthread = Thread(target=self._main, args=())
        self.mainthread.daemon = True
        self.sockthread = Thread(target=self._listen, args=())
        self.sockthread.daemon = True
        self.threads += [self.mainthread, self.sockthread]

    def _update_from_config(self):
        conf = readcfg(self.fnconf)

        # Logger Setup
        self.loglevel = eval(conf[self.section]['loglevel'])
        self._configlogger()

        # Socket Setup
        self.host = conf[self.section]['host']
        self.port = int(conf[self.section]['port'])
        self.sock = socket(AF_INET, SOCK_STREAM)

        return conf

    def _configlogger(self):
        """Update `self.logger` with current loglevel param."""
        self.logger = Logger(__file__)
        handler = StreamHandler(stdout)
        handler.setLevel(self.loglevel)
        formatter = Formatter('%(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        self.logger.debug(f'logger configured to loglevel {self.loglevel}')

    def start(self):
        """Start daemon threads"""
        if self.go:
            self.logger.error('start called but go already True')
            return
        try:
            self.sock.bind((self.host, self.port))
        except sock_error as e:
            print(e)
            self.logger.error(f'start called but {self.host}:{self.port} already in use')
            return
        else:
            self.logger.debug(f'socket bound to {self.host}:{self.port}')
        self.go = True
        for t in self.threads:
            t.start()

    def stop(self, join=False):
        """Stop mainloop thread by sending stop signal to socket.

        Parameters
        ----------
        join : bool, optional
            Whether or not to join instance's run threads, by default False
        """
        with socket(AF_INET, SOCK_STREAM) as sock:
            sock.connect( (self.host, self.port) )
            sock.sendall( b'off' )
        assert not self.go
        if join:
            self.join()

    def join(self):
        if self.go:
            self.logger.warning('join called but go is True')
        for t in self.threads:
            try:
                t.join()
            except Exception as e:
                self.logger.error(f'{e}')
        self.logger.debug('All threads joined')

    def _listen(self):
        """Shutoff command listener thread. Controlled by `self.go` attribute."""
        self.logger.debug('Entering socket listening thread')
        while self.go:
            try:
                self.sock.listen()
                conn, addr = self.sock.accept()
                data = conn.recv(128).decode('utf-8')
                # TODO: log received connections
                if data == 'off':
                    self.go = False
                    self.sock.close()
                    self.logger.info('Shutoff signal recieved. Shutting down...')
                else:
                    self.logger.warning(f'Socket connection sent {data} rather than off')
                    self.fault.value = 1  # turn on fault led to alert user
                self.logger.debug(
                    f'socket connection; conn:{conn} addr:{addr} data:{data} go:{self.go}'
                )
            except Exception as e:
                self._errlog(e)
        self.logger.debug(f'listen thread exited. go:{self.go}')

    def _main(self):
        """Abstract mainloop method"""
        raise NotImplementedError('User must override _main method')


if __name__ == '__main__':
    print('This file contains abstract superclasses')

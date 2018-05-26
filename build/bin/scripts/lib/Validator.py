#!/usr/bin/env python
# encoding: utf-8
"""Positve look-ahead and negative look-behind assertions
"""
import re

class MailValidator(object):
    def __init__(self):
        self.__matched = False
        # An email address: username@domain.tld
        self.__address = re.compile(
            r'''
            # Limit the top-level domains
            (?=.*\.(?:com|org|edu)$)
            ^(
               [\w\d.+-]+   # Account name

               (?<!noreply) # Ignore "noreply"
               @[\w\d.+-]+  # Domain name
            )$
            ''',
            re.UNICODE | re.VERBOSE)

    def update(self, url):
        self.__matched = self.__address.search(url)

    @property
    def Match(self):
        return self.__matched is not None

    @property
    def Result(self):
        if self.__matched is None:
            return None
        return self.__matched.groups()[0]
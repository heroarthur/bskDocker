# coding=utf-8


import argparse
import base64
import binascii
import fnmatch
import os
import platform
import socket
import struct
from Crypto import Random
from Crypto.Cipher import AES
from Crypto.Hash import SHA256
from Crypto.PublicKey import RSA
from Crypto.Util import Counter
from Crypto.Util.number import bytes_to_long, long_to_bytes


MAGIC = 0xBEE4FAC3#4092845167#0xBEE4FAC3

class Decrypter:

	def _decrypt_header(self, filename, input_file, key):
		iv = input_file.read(AES.block_size)

		if len(iv) < AES.block_size:
			raise Exception("Error in data {0}".format(filename))

		ctr = Counter.new(AES.block_size * 8, initial_value=long(binascii.hexlify(iv), 16))
		aes = AES.new(key, mode=AES.MODE_CTR, counter=ctr)

		data = input_file.read(AES.block_size)

		if len(data) < AES.block_size:
			raise Exception("Error in data {0}".format(filename))

		data = aes.decrypt(data)
		if long(binascii.hexlify(data[:4]), 16) != MAGIC:
			raise Exception("Error in data {0}".format(filename))

		lsize = struct.calcsize('<I')
		l = struct.unpack('<I', data[4:4 + lsize])[0]
		filename = str(data[4 + lsize:])
		filename = filename[:l]
		while len(filename) < l:
			data = input_file.read(AES.block_size)
			filename += aes.decrypt(data)
			filename = filename[:l]

		return (aes, filename)

	def _decrypt_file(self, path, filename, key):
		try:	
			with open(os.path.join(path, filename), 'rb') as input_file:
				try:
					aes, output_filename = self._decrypt_header(filename, input_file, key)
				except:
					return

				with open(os.path.join(path, output_filename), 'wb') as output_file:
					while True:
						data = input_file.read(AES.block_size)
						if len(data) == 0:
							break
						output_file.write(aes.decrypt(data))
		except:
			return
		
		print "The file {0} is decrypted ".format(output_filename)
		os.remove(os.path.join(path, filename))

	def process(self, path, digest_key_list):
		lookup = dict(digest_key_list)
		lookup.update(dict((digest[:32], key) for digest, key in digest_key_list))
		for dirpath, _, filenames in os.walk(path):
			for filename in filenames:
				try:
					key = lookup[filename[1:-7]] # skip the file extension .shif
					self._decrypt_file(dirpath, filename, base64.b64decode(key))
				except:
					continue

def proba(filename, count):
	key = "('Linux', 'debian', '3.16.0-4-586', '#1 Debian 3.16.36-1+deb8u1 (2016-09-03)', 'i686', '') 17353156424758832917984251216341773811545352501474881518974872660981150695920"
	key = bytes_to_long(key)+count
	key = long_to_bytes(key)

	sha256digest = SHA256.new(key).digest()	
	path = ''

	#filename = '.8a073945c339990f80402047d0c23cfc.шиф'
	d = Decrypter()	
	d._decrypt_file(path, filename, sha256digest)


def odszyfruj_jeden(filename, ilosc_plikow_do_odszyfrowania):
	for i in range(0, ilosc_plikow_do_odszyfrowania):
		proba(filename, i)


if __name__ == '__main__':
	ilosc_plikow_do_odszyfrowania = 22
	for filename in os.listdir("."):
	    if filename.endswith(".шиф"):
		odszyfruj_jeden(filename, ilosc_plikow_do_odszyfrowania)








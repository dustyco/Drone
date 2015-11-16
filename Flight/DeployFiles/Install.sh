#!/bin/bash

[ "$1" ] && bin_prefix="$1" || bin_prefix="$(pwd)"
[ "$2" ] && src_prefix="$2" || src_prefix="$(pwd)"
[ "$3" ] && out_prefix="$3" || out_prefix=""

echo "====== Binary prefix: $bin_prefix/"
echo "====== Source prefix: $src_prefix/"
echo "====== Installing to $out_prefix/"

canonical_name="DroneFlight"
lowercase_name="drone-flight"
subfolder="Flight"

# Binary
install -Dm755 \
	"$bin_prefix/$canonical_name" \
	"$out_prefix/usr/bin/$canonical_name"

# Systemd service file
install -Dm644 \
	"$src_prefix/$subfolder/DeployFiles/systemd/$canonical_name.service" \
	"$out_prefix/usr/lib/systemd/system/$canonical_name.service"

# Init service file
install -Dm644 \
	"$src_prefix/$subfolder/DeployFiles/init/$lowercase_name.conf" \
	"$out_prefix/etc/init/$lowercase_name.conf"
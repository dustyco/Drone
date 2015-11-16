#!/bin/bash

[ "$1" ] && bin_prefix="$1" || bin_prefix="$(pwd)"
[ "$2" ] && src_prefix="$2" || src_prefix="$(pwd)"
[ "$3" ] && out_prefix="$3" || out_prefix=""

echo "====== Binary prefix: $bin_prefix/"
echo "====== Source prefix: $src_prefix/"
echo "====== Installing to $out_prefix/"

canonical_name"DroneFind"
subfolder="Find"

# Binary
install -Dm755 \
	"$bin_prefix/$canonical_name" \
	"$out_prefix/usr/bin/$canonical_name"
#!/bin/sh

VERSION=0.1

NAME=qt6gtk2

TARBALL=$NAME-$VERSION

mkdir -p extras/package/sources
git archive --format=tar --prefix=$TARBALL/ $VERSION | xz > extras/package/sources/$TARBALL.tar.xz

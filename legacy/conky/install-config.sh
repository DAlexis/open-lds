#!/bin/bash

set -e

mkdir -p ~/.config/autostart
cp conky.desktop ~/.config/autostart
cp .conkyrc ~/.conkyrc

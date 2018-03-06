#!/bin/bash

set -e

echo "This is dfclient total installation script."
echo "Usage: Run this on a new computer's 'boltek' user home directory"

echo "== Updating system and installing required packages"

sudo apt-get update
sudo apt-get -y upgrade
sudo apt-get -y install vim vim-gtk vim-pathogen mc git htop mercurial subversion samba gcc g++ libboost-all-dev tmux mariadb-server mariadb-client openssh-server geany cmake libmysqlclient-dev libgtest-dev python3-mysql.connector conky python3-crypto

echo "== Installing vim plugins"
(
#install vim nerdtree
    mkdir -p ~/.vim/bundle
    cd ~/.vim/bundle
    git clone https://github.com/scrooloose/nerdtree.git
    cd ~/
# activate pathogen
    echo "execute pathogen#infect()" >> .vimrc
    echo "call pathogen#helptags()" >> .vimrc
    echo "syntax on" >> .vimrc
    echo "filetype plugin indent on" >> .vimrc
) || true


#Build GTest
if [ ! -f /usr/lib/libgtest.a ] ; then
	echo "== Building gtest"
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
 
# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
    sudo cp -f libgtest.a libgtest_main.a /usr/lib
fi

# Get lightning-df repository and build
echo "== Clonning git repo"
if [ ! -d lightning-df ] ; then
    git clone https://bitbucket.org/D_Alexis/lightning-df.git
fi

cd lightning-df

git checkout master
git pull origin master
git submodule init
git submodule update

# build
echo "== Building and installing dfclient"
pwd
./install-dfclient.sh

# ssh keys

echo "== Copying ssh identity to server"
if [ ! -f ~/.ssh/id_rsa.pub ] ; then
	ssh-keygen -t rsa
fi

ssh-copy-id -i ~/.ssh/id_rsa geo-provider@diogen.lightninglab.ru

crontab cpp-sources/dfclient/cronjob

echo "!! Do not forget to add one row to server database with actual device_id"

#cd ~/
#git clone https://xredor@bitbucket.org/xredor/hypernet.git
#cd hypernet/hclients-py
#./hworker.py -keyren
#./hworker.py -register

FROM debian:jessie
MAINTAINER Michal Zak <mz347296@students.mimuw.edu.pl>

USER root

RUN groupadd -g 1357 zpp
RUN useradd -g 1357 vagrant

# add vagrant user to the sudo group
RUN usermod -aG sudo vagrant

# install ssh key for vagrant user
RUN mkdir -p /home/vagrant/.ssh
COPY vagrant_id_rsa.pub /home/vagrant/.ssh/authorized_keys

# fix permissions
RUN chown -R vagrant /home/vagrant/
RUN chmod 0600 /home/vagrant/.ssh/authorized_keys
RUN chmod 0700 /home/vagrant/.ssh

# install apps
RUN apt-get update \
    && apt-get install -y \
    g++ \
    openssh-server \
    wget \
    curl \
    sudo \
    make \
    cmake \
    tree \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# clone gmock
RUN mkdir -p /home/vagrant/gmock-1.7.0
RUN git clone https://github.com/paulsapps/gmock-1.7.0.git /home/vagrant/gmock-1.7.0

# configure shell
RUN usermod -s /bin/bash vagrant
RUN echo "umask 002" >> /home/vagrant/.bashrc
RUN echo "if [ -f ~/.bashrc ]; then . ~/.bashrc; fi" >> /home/vagrant/.bash_profile
RUN chown vagrant /home/vagrant/.bashrc

# allow passwordless sudo
RUN echo "%sudo ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

RUN mkdir -p /var/run/sshd

EXPOSE 22

CMD /usr/sbin/sshd -D

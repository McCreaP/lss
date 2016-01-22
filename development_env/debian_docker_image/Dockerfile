FROM debian:jessie
MAINTAINER Michal Zak <mz347296@students.mimuw.edu.pl>

USER root

RUN groupadd -g 1357 zpp
RUN useradd -g 1357 --create-home --shell /bin/bash vagrant

# add vagrant user to the sudo group
RUN usermod -aG sudo vagrant

# install ssh key for vagrant user
RUN mkdir -p /home/vagrant/.ssh
COPY vagrant_id_rsa.pub /home/vagrant/.ssh/authorized_keys

# fix permision
RUN chmod 0700 /home/vagrant/.ssh
RUN chmod 0600 /home/vagrant/.ssh/authorized_keys

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
    python \
    python3 \
    python3-pip \
    unzip \
    lcov \
    bc \
    libpstreams-dev \
    libgoogle-glog-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install colorlog
RUN pip3 install bottle

# add cpplint
ADD https://raw.githubusercontent.com/McCreaP/styleguide/gh-pages/cpplint/cpplint.py /usr/local/bin/
RUN chown vagrant:zpp /usr/local/bin/cpplint.py
RUN chmod +x /usr/local/bin/cpplint.py

# install google test
ADD https://googletest.googlecode.com/files/gtest-1.7.0.zip /home/vagrant/
RUN unzip /home/vagrant/gtest-1.7.0.zip -d /home/vagrant
RUN cd /home/vagrant/gtest-1.7.0 && mkdir build && cd build && cmake .. && make
RUN cp -r /home/vagrant/gtest-1.7.0/include/gtest /usr/local/include
RUN cp /home/vagrant/gtest-1.7.0/build/lib*.a /usr/local/lib
RUN rm -rf /home/vagrant/gtest-1.7.0 && rm /home/vagrant/gtest-1.7.0.zip

# install google mock
ADD https://googlemock.googlecode.com/files/gmock-1.7.0.zip /home/vagrant/
RUN unzip /home/vagrant/gmock-1.7.0.zip -d /home/vagrant
RUN cd /home/vagrant/gmock-1.7.0 && mkdir build && cd build && cmake .. && make
RUN cp -r /home/vagrant/gmock-1.7.0/include/gmock /usr/local/include
RUN cp /home/vagrant/gmock-1.7.0/build/lib*.a /usr/local/lib
RUN rm -rf /home/vagrant/gmock-1.7.0 && rm /home/vagrant/gmock-1.7.0.zip

# allow passwordless sudo
RUN echo "%sudo ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# configure bashrc
RUN echo "if [ -f ~/.bashrc ]; then . ~/.bashrc; fi" >> /home/vagrant/.bash_profile
COPY bashrc /home/vagrant/.bashrc
RUN chown -R vagrant:zpp /home/vagrant

RUN mkdir -p /var/run/sshd

EXPOSE 22

CMD /usr/sbin/sshd -D

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

# fix permissions
RUN chown -R vagrant:zpp /home/vagrant/.ssh
RUN chmod 0700 /home/vagrant/.ssh
RUN chmod 0600 /home/vagrant/.ssh/authorized_keys

# fix bash default interpreter
RUN rm /bin/sh && ln -s /bin/bash /bin/sh

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
    libboost-all-dev \
    vim \
    less \
    git \
    php5-cli \
    php5-curl \
    cppcheck \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install colorlog
RUN pip3 install bottle
RUN pip3 install pylint
RUN pip3 install flake8

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

# install arcanist
RUN mkdir /home/vagrant/arcanist
RUN git clone https://github.com/phacility/libphutil.git /home/vagrant/arcanist/libphutil
RUN git clone https://github.com/phacility/arcanist.git /home/vagrant/arcanist/arcanist
ENV PATH "/home/vagrant/arcanist/arcanist/bin:$PATH"

# allow passwordless sudo
RUN echo "%sudo ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# add binaries and scripts to PATH
ENV PATH "/home/vagrant/lss/bin:/home/vagrant/lss/scripts:$PATH"

# configure bashrc
RUN echo "if [ -f ~/.bashrc ]; then . ~/.bashrc; fi" >> /home/vagrant/.bash_profile
COPY bashrc /home/vagrant/.bashrc
RUN echo export PATH=$PATH >> /home/vagrant/.bashrc
RUN cat /home/vagrant/arcanist/arcanist/resources/shell/bash-completion >> /home/vagrant/.bashrc
RUN chown -R vagrant:zpp /home/vagrant

RUN mkdir -p /var/run/sshd

EXPOSE 22

CMD /usr/sbin/sshd -D

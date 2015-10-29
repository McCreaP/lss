# Setting up development environment

## Vagrant 

First, you need to install the latest version of Vagrant. Deb and rpm packages are available [here](https://www.vagrantup.com/downloads.html). More information about Vagrant can be found at https://www.vagrantup.com/

## Docker

You will also need [Docker](https://www.docker.com/). Type the command: 

`wget -qO- https://get.docker.com/ | sh`. 

I suggest adding your user to the "docker" group: 

``sudo usermod -aG docker `whoami` ``

You can verify if docker has been installed correctly: 

`docker run hello-world`.

## Group zpp

Add your user to the group "zpp": 

`sudo groupadd -g 1357 zpp`

``sudo usermod -g zpp `whoami` ``

# Usage manual

## Basic information

In the **development_env** directory, the command `vagrant up` will build and run a docker container defined in **development_env/debian_docker_image/Dockerfile**. You can SSH into this container with `vagrant ssh`.

Inside the docker container the directory **~/lss/** is synchronized with the git root directory on the host machine. The idea is to develop locally on your machine and compile / run programs inside the docker container, so all dependencies should be installed there.

Be aware that changes made inside the docker container are not persistent! Reloading the container will erase these changes. To make them persistent you should edit the **Dockerfile**.

## Other useful commands

* `vagrant destroy` - this command stops the running container Vagrant is managing and destroys all resources that were created during the machine creation process.
* `vagrant reload` - this command is required for changes made in the **Vagrantfile** and the **Dockerfile** to take effect.

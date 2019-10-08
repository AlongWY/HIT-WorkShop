FROM base/archlinux

MAINTAINER "Alongwy <mailto:alongwyforever@outlook.com>"

RUN sed -i '1i\Server = https://mirrors.tuna.tsinghua.edu.cn/archlinux/$repo/os/$arch' /etc/pacman.d/mirrorlist && \
    pacman -Sy --noconfirm && \
    pacman -Syy --noconfirm && \
    pacman -S jdk10-openjdk \
              python \
              python-pip \
              python-numpy \
              python-matplotlib \
              python-pandas \
              python-openpyxl \
              jupyter-notebook --noconfirm && \
    pacman -Scc --noconfirm && \
    pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple && \
    pip install --no-cache-dir pyspark 

FROM gcc:4.9

COPY . /usr/src/traveler
WORKDIR /usr/src/traveler

RUN \
  cd /usr/src/traveler/src && \
  make build

ENV PATH /usr/src/traveler/bin:$PATH

ENTRYPOINT ["/bin/bash"]

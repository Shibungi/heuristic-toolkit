FROM gcc:15.2.0-bookworm
RUN apt-get update && apt-get install -y --no-install-recommends python3 python3-venv util-linux \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /work
COPY . /opt/heuristic-toolkit
ENV PATH="/opt/heuristic-toolkit/environment:${PATH}"
CMD ["bash"]

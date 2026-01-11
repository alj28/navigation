#!/bin/bash

echo Activate virtual environment
source .venv/bin/activate

echo Open VS code
code .

export PRJ_PATH="$(pwd)"

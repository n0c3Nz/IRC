#!/bin/bash

{
  echo "PASS 12345"
  echo "NICK paco"
  echo "USER paquito 0 * :pelotudo"
  cat  # Mantiene la entrada abierta para interactuar con el proceso
} | nc 127.0.0.1 6660


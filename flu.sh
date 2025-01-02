#!/bin/bash

PORT=6660

# Busca el PID del proceso que está usando el puerto
pid=$(lsof -t -i:${PORT})

if [ -n "$pid" ]; then
  echo "Matando proceso con PID $pid que está usando el puerto $PORT..."
  kill -9 "$pid"
  echo "Proceso eliminado."
else
  echo "No hay ningún proceso usando el puerto $PORT."
fi


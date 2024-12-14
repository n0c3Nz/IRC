# ft\_irc

## Descripción del Proyecto

**ft\_irc** es un proyecto para desarrollar un servidor de Internet Relay Chat (IRC) en **C++ 98**. El servidor debe permitir a los clientes conectarse y comunicarse en tiempo real a través del protocolo IRC, cumpliendo con las especificaciones establecidas en el documento del proyecto.

---

## Requisitos

### General:

- El programa no debe bloquearse ni salir inesperadamente bajo ninguna circunstancia.
- Debe entregarse un **Makefile** con las siguientes reglas obligatorias:
  - `all`
  - `clean`
  - `fclean`
  - `re`
- Debe compilarse con **C++ 98** y las flags:
  - `-Wall -Wextra -Werror`
- Uso de funciones externas permitido:
  - `socket`, `close`, `setsockopt`, `getsockname`, `getprotobyname`, `gethostbyname`, `getaddrinfo`, `freeaddrinfo`, `bind`, `connect`, `listen`, `accept`, `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`, `send`, `recv`, `signal`, `sigaction`, `lseek`, `fstat`, `fcntl`, `poll` (o equivalente).

### Funcionalidades Obligatorias:

- **Autenticación**: Los clientes deben autenticarse usando un nickname y un username.
- **Conexión y comunicación**: Los clientes deben poder:
  - Conectarse al servidor utilizando un puerto y contraseña.
  - Unirse a canales.
  - Enviar y recibir mensajes privados.
  - Enviar mensajes a canales, los cuales se retransmitirán a todos los miembros del canal.
- **Roles**:
  - Usuarios regulares.
  - Operadores de canal con comandos específicos:
    - `KICK`: Expulsar a un usuario del canal.
    - `INVITE`: Invitar a un usuario al canal.
    - `TOPIC`: Cambiar o ver el tema del canal.
    - `MODE`: Cambiar el modo del canal.
      - `i`: Canal solo por invitación.
      - `t`: Restringir cambios de tema a operadores.
      - `k`: Establecer una contraseña.
      - `o`: Otorgar o quitar privilegios de operador.
      - `l`: Establecer un límite de usuarios.

---

## Ejecución

El servidor se ejecuta de la siguiente manera:

```
./ircserv <puerto> <contraseña>
```

- `<puerto>`: Número de puerto en el que el servidor escuchará conexiones.
- `<contraseña>`: Contraseña requerida para que los clientes se conecten.

---

## Bonus

Si se completan todas las funcionalidades obligatorias de forma perfecta, se pueden agregar las siguientes características:

- Transferencia de archivos.
- Un bot para automatizar acciones en el servidor.

---

## Estructura del Proyecto

- **Makefile**: Contiene las reglas para compilar y limpiar el proyecto.
- **Archivos fuente**: Código del servidor IRC desarrollado en C++ 98.
- **Opcional**: Archivo de configuración para personalizar el comportamiento del servidor.

---

## Referencia del Cliente IRC

El cliente IRC de referencia se utilizará para evaluar el servidor. Asegúrate de probar exhaustivamente el servidor con este cliente antes de la evaluación.

---


## Uso de `nc` para Pruebas

Puedes usar el comando `nc` para realizar pruebas básicas de tu servidor:

```
$> nc -C 127.0.0.1 6667
com^Dman^Dd
$>
```

- Usa `Ctrl+D` para enviar los datos en partes.


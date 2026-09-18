# Cachet

**Cachet** (*French: seal*) is an extensible [Bend](https://bend-lang.com) library for **JWT authentication** (HS256). Native effects use **OpenSSL**; orchestration is pure Bend.

MIT licensed.

## Features

- HS256 sign and verify
- Claims: `sub`, `iss`, `aud`, `exp`, `iat`
- Bearer header parsing
- Issuer, audience, and expiry validation
- Extensible via `Auth.on_authorized` hook

## Dependencies

- [Bend](https://bend-lang.com/install.sh) 2.x
- OpenSSL (`brew install openssl@3`)
- clang 14+

## Quick start

```bash
make proof
make check
make example
```

## Bend Hub

Published on [hub.bend-lang.com](https://hub.bend-lang.com):

```python
import 0xac19a5c9b4cef01e72dcbe9e5b881fc7/cachet.bend as Cachet
import 0x5aa4baca6ab29461acfb0fd1dc5ebd4d/types.bend as Types
import 0x22403a77ee8da085cf4e1458c693703c/claims.bend as Claims
import 0xc383c1b95391d033028a0003fad190e5/jwt.bend as JWT
import 0x184803968dae9580005e472b803ca2f1/auth.bend as Auth
```

Republish after changes:

```bash
make publish
NODE_TLS_REJECT_UNAUTHORIZED=0 bend types.bend --publish
NODE_TLS_REJECT_UNAUTHORIZED=0 bend claims.bend --publish
```

## Usage

```python
import Base
import 0x5aa4baca6ab29461acfb0fd1dc5ebd4d/types.bend as Types
import 0x22403a77ee8da085cf4e1458c693703c/claims.bend as Claims
import 0xc383c1b95391d033028a0003fad190e5/jwt.bend as JWT
import 0x184803968dae9580005e472b803ca2f1/auth.bend as Auth
import 0xac19a5c9b4cef01e72dcbe9e5b881fc7/cachet.bend as Cachet

def main() -> IO(Unit):
  do IO<Unit>:
    +now : U32 <- Cachet.Clock.unix_now()
    +claims : Types.Claims =
      Types.C{"user-1", "my-app", "api", U32.add(now, 3600), now}
    +secret : String = "your-secret"
    +token : String <- IO.try(String, JWT.Sign.token(claims, secret))
    +header : String = String.append("Bearer ", token)
    verified : Types.Claims <- IO.try(
      Types.Claims,
      Auth.Guard.authorize(header, secret, "my-app", "api"))
    IO.print(Claims.Access.sub(verified))
```

## Modules

| Module | Role |
|--------|------|
| `cachet.bend` | Crypto effects (base64url, HMAC, clock, pipe) |
| `types.bend` | `Claims`, `Token`, `TokenParts` |
| `claims.bend` | `Access`, `Json`, `Parse`, `Validate` |
| `jwt.bend` | `Sign`, `Verify`, `Decode` |
| `auth.bend` | `Guard.authorize`, bearer parsing |

## Extend

Override `Auth.on_authorized` in your app module for custom post-auth logic.

## License

MIT

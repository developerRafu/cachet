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

Publish:

```bash
make publish
```

Import in your project:

```python
import 0x<hash>/cachet.bend as Cachet
import 0x<hash>/claims.bend as Claims
import 0x<hash>/jwt.bend as JWT
import 0x<hash>/auth.bend as Auth
```

## Usage

```python
import Base
import 0x<hash>/types.bend as Types
import 0x<hash>/jwt.bend as JWT
import 0x<hash>/auth.bend as Auth
import 0x<hash>/cachet.bend as Cachet

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

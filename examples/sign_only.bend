import Base
import ../types.bend as Types
import ../jwt.bend as JWT
import ../cachet.bend as Cachet

def main() -> IO(Unit):
  do IO<Unit>:
    +now : U32 <- Cachet.Clock.unix_now()
    +claims : Types.Claims =
      Types.C{"user-1", "cachet-demo", "api", U32.add(now, 3600), now}
    +secret : String = "change-me-in-production"
    +signed : String <- IO.try(String, JWT.Sign.token(claims, secret))
    IO.print(signed)

When using Bend:
- run `bend guide` to learn it
- use `LAWS.bend` to keep important rules
- run `bend PROOF.bend` before committing
- parallelize the code whenever possible

Cachet is a JWT auth library for Bend (HS256). Extend behavior by overriding
`Auth.on_authorized` in your app module.

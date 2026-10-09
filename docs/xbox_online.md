# Online play on the original Xbox: options and recommendation

Research only (agent A, 9 Oct 2026). No code changed, no service contacted, nobody was written to. Web pages were
read as data. Every fact is tagged: **[web]** read at the cited page, **[docs]** from this repo, **[known]** general
knowledge not re-checked today, **[guess]** inference. Several details (how Insignia authenticates titles, how a title
gets added, the system link wire format) were **not findable** on public pages; they are listed as open questions
(section 6) rather than guessed.

## 1. The original Xbox online today

### 1.1 Insignia (Xbox Live 1.0 revival)

| Fact | Source |
|---|---|
| Free, non-commercial replacement for Microsoft's original Xbox Live servers, made by reverse engineering the Live server software. Closed source, hosted by the Insignia team. Public beta. | https://en.wikipedia.org/wiki/Insignia_(Xbox), https://insignia.live/ |
| Works on unmodified consoles (a one-time USB "endgame" exploit runs a registration tool), modded consoles (tool copied to the HDD) and xemu. Needs an email and an Insignia account; no fee, no ads. | same Wikipedia page |
| Features reimplemented: matchmaking, leaderboards, friends, invites, clans, user-generated content, voice chat; some titles get updates / DLC. | same |
| About 209 of the 381 Live titles supported (Wikipedia, Oct 2026); the site says 210 and its list shows about 180 serials. Support is added "on an ad-hoc basis"; "some titles require more server functionality than others". Capcom titles on the list: Auto Modellista, Capcom Fighting Evolution, Capcom vs. SNK 2 EO, Darkwatch, Street Fighter Anniversary Collection, Steel Battalion LoC (US and JP). Phantasy Star Online, Halo 2, PGR2, Burnout 3 also. | https://insignia.live/games |
| Homebrew: **none listed** on the games page. No public policy on homebrew or unofficial titles was found. Contact route seen in sources: the Insignia Discord (link on the homepage). | https://insignia.live/games, https://insignia.live/ |
| DNS / authentication internals were **not found** on any page fetched. The Insignia homepage links a Connection Guide, a NAT guide and a ticket-cache guide that the fetch tool could not read; the direct guesses at their URLs returned 404. | - |
| What I believe, **[known]/[guess]**, not verified today: consoles are pointed at Insignia's DNS, which steers the Live hostnames (`*.xboxlive.com`-style names the XDK's XOnline library uses) to Insignia servers; a console is registered once so it holds a Live-style machine account, and users get gamertags on top. A title is "supported" when the servers answer the specific Live services that title's retail build calls (authentication, matchmaking, stats) the way Microsoft's did. | - |

Related tools on the Insignia side:
* `insignia-live/setup-assistant-release`: an Xbox homebrew tool that "sets up and partially tests your Xbox's
  connection to Insignia's servers". https://github.com/insignia-live/setup-assistant-release
* `insignia-live/xblunblock`: an nxdk-built XBE (release notes mention an nxdk bug) that Insignia ships; purpose not
  stated on the page read (**unknown**). https://github.com/insignia-live/xblunblock/releases
* xb.live (a **third-party** site, "not affiliated with Microsoft or Insignia"): stats, friends, and a **Homebrew SDK for
  C / nxdk** for leaderboards: you register a game, get an approved signing secret, call `/api/hb/run/start` and
  `/api/hb/score`; the server replays input logs to verify scores; logins are Insignia accounts. It is an HTTP API, not
  XOnline. https://xb.live/ecosystem. This shows an nxdk title can already make **Insignia-linked HTTP calls**, but only
  for leaderboards, not matchmaking.

### 1.2 XLink Kai and other options

* XLink Kai runs on a PC / Mac / Linux box on the same LAN as the console. When the console's game starts a system
  link search, the traffic is captured by Kai and tunnelled over the internet to other Kai users, so the consoles
  see each other as if on one LAN. Free tier plus paid supporter tier **[known]**; terms at teamxlink.co.uk (not read).
  Original Xbox is its home use case; any title with a system link mode works, no Xbox Live needed.
  Latest stable listed on Wikipedia: 7.4.45 (Nov 2023). https://en.wikipedia.org/wiki/XLink_Kai
* Kai is a **title-agnostic LAN tunnel**: a homebrew title that speaks LAN broadcast on the right ports appears to
  work the same as a retail one **[guess, untested]**. Kai tracks per-game "arenas"; a new game may be listed
  under a generic arena or need adding (ask the Kai team) **[guess]**.
* xemu can do system link over its own network backends (pcap/UDP tunnel) **[known]**, useful for our testing.
* Other: plain port-forwarded or VPN (Tailscale / ZeroTier / WireGuard) LANs work for a few friends **[known]**.
  No other Live revival of comparable size was found; Insignia is the one.

### 1.3 What an nxdk homebrew title can use

| Capability | Status |
|---|---|
| TCP/UDP sockets | nxdk ships `lib/net`, "a network stack for the Xbox based on lwIP", with BSD-socket style APIs. lwIP is Modified BSD. https://github.com/XboxDev/nxdk. NevolutionX (an nxdk dashboard) runs an FTP server over it and needs only a link and a DHCP server. https://github.com/dracc/NevolutionX |
| Name resolution, DHCP | lwIP DNS/DHCP are normally part of that setup **[known]**; confirm in `lib/net` when we start. |
| Broadcast / LAN | lwIP supports UDP broadcast **[known]**; the nforce NIC driver is in nxdk **[known]**. Needs a quick test. |
| Xbox Live APIs (XOnline) | The XDK's XOnline library is Microsoft's, not redistributable, and nxdk cannot link it **[known]**. Using leaked XDK libraries in a public repo is a legal no. Reimplementing the Live wire protocol ourselves would be a big reverse-engineering project and a Microsoft-IP risk: **not an option** here. |
| System link as a retail game does it | Retail system link uses XNet (the XDK's socket layer) with its own discovery messages (I believe UDP, port 3074-ish **[unverified; two searches found no wire-format description]**). We do not need that format: our direct-connect code uses its own TCP (section 2b). |

nxdk is not itself on the Live "allow list"; since Insignia works by redirecting names for the XOnline stack, a plain
lwIP socket gets **no Live services**, only a normal IP connection **[guess, consistent with the above]**.

## 2. Options for MH1 on Xbox

Our assets **[docs: network.md]**: the game's own lobby client (`cnet`/`cnLBS_*`) runs on PC over `CpInet*` TCP, tested
only against `tools/mh1_testserver.py` (359 lines, loopback, lenient). It reaches the lobby (member list, chat) but not
rooms, matching, a game session (`mcsls`) or the portal. The Xbox build is meant to use the same boundary with nxdk
lwIP (`net_cpinet.c` has a Winsock path, not compiled). Co-op over direct connect (`net_peer.c`) is TCP, port 10300,
host relays, 2-4 players, private addresses by default, tested on loopback only; the real game has no peer-to-peer
(no UDP, no connection to another player's address), so this is our own design.

### (a) Xbox talks to MH Oldschool's PS2 servers directly

* **How:** same code as the PC build; the Xbox needs name resolution that sends the game's hostnames to MH Oldschool.
  PS2 players change primary DNS to MH Oldschool's server [docs: network.md 2.1]. On Xbox we would either set the
  console DNS to theirs (works with nxdk's resolver) or hard-code their addresses.
* **Feasibility: not today.** Missing before this could even work: the portal (HTTPS to `*.kddi-mmbb.jp`, with Sony-era
  certificates and `mmbb://` commands; network.md section 7 item 2), DNAS handling (network.md section 7 item 4),
  rooms / matching / game session (item 3), and the ports (unknown). Even the best case is a PC-like level of progress
  that the PC build has not reached. TLS on nxdk is possible (NevolutionX does TLS) **[web]** but old cipher suites
  would need checking **[guess]**.
* **Effort:** high: the missing protocol pieces are the same for PC, so Xbox adds only the port of sockets
  (small, days) **[guess]**. The big work is shared with the PC.
* **Legal / ToS:** MH Oldschool is a fan service for NTSC-J PS2 discs; the repo rule is **no connection without their
  permission**. The draft message exists (network.md section 8). They may also object to non-PS2 clients or to the load.
  Capcom's IP is unaffected (we ship no game data).
* **Who must agree:** MH Oldschool operators (and the owner to send the message). Nothing else.

### (b) System link / direct connect co-op over LAN or XLink Kai

* **How:** the existing `net_peer.c` over nxdk sockets: one Xbox hosts, others join by IP. For a LAN a discovery
  step (UDP broadcast "host here" on a fixed port) would be nicer than typing IPs; the Xbox has no keyboard, so an
  on-screen IP picker or discovery is a must.
* **XLink Kai:** the Kai client on a PC sees broadcast traffic from the console. Whether Kai tunnels our non-standard
  traffic depends on Kai's capture rules (it sniffs known system-link traffic) **[guess]**: **untested, assume it
  works only if our discovery looks like system link**, otherwise players need a VPN (Tailscale / ZeroTier gives
  the same effect with no Kai). Both are free for a few friends **[known]**.
* **Feasibility: high.** Only needs lwIP sockets and a join UI. Cross-play with PC is automatic (same code and
  protocol), which is a real advantage over Live-based options.
* **Effort:** low to medium: sockets port (days), host/join UI (days), discovery (days), plus testing on hardware.
  Remaining risk is the unfinished co-op itself (it is proven on loopback only).
* **Legal / ToS:** none for our own protocol. Kai terms apply to its users, not to us.
* **Who must agree:** nobody. The owner decides.

### (c) Appearing in Insignia

* **How it would work:** Insignia supports titles that call Microsoft's XOnline services. Our title would have to
  call Live services via the official XDK libraries, which we cannot link (section 1.3). The only way to "appear" is to
  make Insignia's servers support a new title ID that talks Live matchmaking, which we cannot generate without the
  XDK and a signing path. **[guess]**
* **Feasibility: very low.** No homebrew is on their list [web]; no policy was found; I could not read how they add
  titles. Their games page says titles are tested and released ad hoc, and the team is small [web].
* **Possible half-measure:** use the xb.live-style HTTP APIs (Insignia login for identity, leaderboards): real and open
  to nxdk **[web]**, but it gives accounts and stats, not matchmaking or relay. It is a post-v1 nicety.
* **Legal / ToS:** using Microsoft XDK binaries or Live protocol docs would be a problem for a public repo.
  Insignia's own terms for homebrew: unknown.
* **Who must agree:** the Insignia team (contact via their Discord). I contacted no one.

### (d) Our own small lobby + relay server

* **What it implements:**
  1. The PS2 lobby protocol the game client speaks (network.md section 5): framing, login, mini data, top info, plazas, lobbies,
     chat, notices. Already done to the "inside a lobby" level in `tools/mh1_testserver.py`.
  2. What is still missing: rooms ("pieces"), matching, and the in-game session (`mcsls`, magic 0x82) which the game
     does through a server (the real game has no peer-to-peer); patches, personal data, mail, user search.
  3. A portal replacement: either a native menu in the port (so no HTTPS pages), or a minimal HTTP(S) server.
  4. Accounts: a user table and password check (the test server accepts any password; a real one needs a store and rate
     limiting, plus a gamertag / save-name policy).
  5. For co-op only: a simple relay for players behind NAT (the existing host-relays design is the same idea, moved
     to a server).
* **Hosting cost:** tiny. Python/Go on a small VPS with 1 vCPU / 1 GB runs hundreds of lobby players; roughly
  3-6 USD/month **[guess/known pricing range]**. Or free: the owner's home PC with port forwarding, which exposes a home IP.
* **Effort:** lobby plus relay for co-op: medium (the lobby is mostly done; sessions are the unknown). A full copy of the
  PS2 online (rooms, matching, quests and downloads, events) is **large** and needs lots of protocol RE; MH Oldschool
  already spent years on it **[guess]**.
* **Legal / ToS:** no Capcom data or code on the server if players keep their own discs; the protocol is from our own client
  analysis. Running a community server is gray for Capcom IP like any private server **[not legal advice]**; keep
  it free, non-commercial, no game data. User data (names, passwords, IPs) means a privacy duty for whoever hosts.
* **Who must agree:** the owner (hosting, being the operator) or a community volunteer. No one else.

## 3. Cross-play

| Pair | Via MH Oldschool | Via our own server | Via direct connect |
|---|---|---|---|
| PC <-> Xbox | Yes if (a) works, same wire protocol and same server | Yes (same protocol) | Yes (same code, same port) |
| Xbox <-> real PS2 | Only through MH Oldschool, which PS2 players already use | Only if our server speaks to PS2 clients too: the PS2 needs DNS pointed at our server and its DNAS/portal flow handled. DNAS check on the console cannot be skipped from our side without a patch on the PS2, which MH Oldschool solved with DNS only [docs] | No (PS2 has no peer-to-peer in this game) |
| PC <-> real PS2 | As above | Same limit | No |

So real PS2 players are reachable **only** through MH Oldschool's servers or a server that imitates them. Our own server
would host a **separate** community (PC + Xbox) unless it also works with a PS2 DNS redirect (a stretch **[guess]**).
Even with MH Oldschool, game-data differences matter: PC/Xbox use the same NTSC-J game logic, so no mismatch is expected,
but any divergence in physics or timing between our decompiled code and the PS2's could desync mixed sessions
(server-authoritative checks for the PS2 side are unknown). **[guess]**

## 4. Summary table

| Option | Feasibility | Effort | Legal risk | Needs agreement |
|---|---|---|---|---|
| (a) MH Oldschool direct | blocked until portal + sessions exist | high (shared with PC) | repo rule: need permission | MH Oldschool |
| (b) LAN / XLink Kai / VPN co-op | high | low-medium | none | nobody |
| (c) Insignia | very low | very high / uncertain | XDK libs not usable | Insignia team |
| (d) own server | medium for lobby + relay; low for full PS2 clone | medium to large | gray, non-commercial | owner (or volunteer) |

## 5. Recommendation

1. **Do (b) first.** Finish the nxdk side of `net_peer.c` (lwIP sockets, host/join UI, optional broadcast discovery),
   test in xemu with its networking, then on hardware. Document "use LAN, XLink Kai or a VPN". This is the only
   option fully in our control and it gives PC-Xbox cross-play for free.
2. **Keep (a) alive without building toward it blindly.** Send the draft message in network.md section 8 *when the owner
   chooses*; its answers (ports, DNAS, portal) decide whether (a) is realistic. Meanwhile keep the `CpInet*` boundary,
   so the lobby client works on Xbox the day permission and protocol knowledge arrive.
3. **Defer (d)** to a small relay only if (b) shows NAT is the main pain point (a relay for co-op is a few hundred lines
   and a cheap VPS). Do not build a full PS2 lobby clone; MH Oldschool already is one.
4. **Skip (c)** for matchmaking. Optional later: xb.live-style Insignia login/leaderboards over HTTP, which nxdk can do.

## 6. Open questions (not answered by public pages today)

* How Insignia authenticates consoles and titles (DNS targets, certificates) and what it requires of a new title ID.
* Insignia's policy on homebrew (ask on their Discord, owner's decision).
* Whether XLink Kai forwards a homebrew title's traffic, and the exact system link discovery wire format.
* nxdk `lib/net` specifics: DHCP/DNS availability, broadcast sending, and whether TLS for the MH portal is feasible.
* MH Oldschool's ports, DNAS expectations and stance on non-PS2 clients (network.md section 8).

## Sources

* https://en.wikipedia.org/wiki/Insignia_(Xbox)
* https://insignia.live/ and https://insignia.live/games
* https://github.com/insignia-live/setup-assistant-release
* https://github.com/insignia-live/xblunblock/releases
* https://xb.live/ecosystem
* https://en.wikipedia.org/wiki/XLink_Kai
* https://github.com/XboxDev/nxdk and https://github.com/dracc/NevolutionX
* docs/network.md, docs/xbox.md, tools/mh1_testserver.py (this repo)

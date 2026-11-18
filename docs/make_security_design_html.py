#!/usr/bin/env python3
"""Writes docs/security_design.html, a standalone copy of the security
design white paper for the release packages, from docs/security_design.php
(the web site page, https://datareel.com/encrypted_password_database/security_design.php).
Run from the repository root: python3 docs/make_security_design_html.py
"""
import os, re, sys
d = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(d, 'security_design.php'), encoding='utf-8').read()
title = re.search(r"\$dr2_t = '([^']*)'", src).group(1)
desc = re.search(r"\$dr2_d = '([^']*)'", src).group(1)
style = re.search(r'<style>(.*?)</style>', src, re.S).group(1)
body = re.search(r'(<article class="dr2-doc">.*?</article>)', src, re.S).group(1)
if '<?' in body:
    sys.exit('the article holds PHP code')
# Site links become full URLs; the README is next to docs/ in the packages
body = body.replace('href="help/README.txt"', 'href="../README.txt"')
body = re.sub(r'href="/', 'href="https://datareel.com/', body)
page = f'''<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>{title}</title>
<style>
:root {{ --dr2-text: #1d2433; --dr2-muted: #5b6475; --dr2-surface-2: #f2f4f8; --dr2-border-strong: #9aa3b2;
        --dr2-accent: #2f6fdb; --dr2-accent-soft: #e6eefc; --dr2-link: #2f6fdb; }}
@media (prefers-color-scheme: dark) {{ :root {{ --dr2-text: #e6e9ef; --dr2-muted: #a3abb9; --dr2-surface-2: #1f2633;
        --dr2-border-strong: #5c6678; --dr2-accent: #7aa7ff; --dr2-accent-soft: #1d2c48; --dr2-link: #7aa7ff; }}
  body {{ background: #11151d; }} }}
body {{ margin: 0 auto; max-width: 900px; padding: 24px 16px 48px; color: var(--dr2-text);
       font: 16px/1.55 system-ui, -apple-system, "Segoe UI", sans-serif; }}
a {{ color: var(--dr2-link); }} code {{ font-size: 0.92em; }}
h1 {{ font-size: 28px; margin-bottom: 4px; }} .lead {{ color: var(--dr2-muted); margin-top: 0; }}
{style}
</style></head><body>
<h1>{title}</h1>
<p class="lead">{desc}</p>
{body}
<p class="lead">Encrypted Password Database, https://datareel.com/encrypted_password_database/security_design.php</p>
</body></html>
'''
open(os.path.join(d, 'security_design.html'), 'w', encoding='utf-8').write(page)
print('wrote docs/security_design.html', len(page), 'bytes')

#!/usr/bin/env python3
import http.server
import socketserver
import json
import os
import sys
import hashlib

DEFAULT_PORT = 5050
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
GUI_DIR = os.path.join(BASE_DIR, 'gui')
DATA_DIR = os.path.join(BASE_DIR, 'data')
CUSTOMERS_FILE = os.path.join(BASE_DIR, 'customers.txt')

os.makedirs(DATA_DIR, exist_ok=True)

def hash_password(password: str) -> str:
    salted = f"cms_salt_v2_{password}"
    return hashlib.sha256(salted.encode('utf-8')).hexdigest()

def get_user_contacts_path(username: str) -> str:
    safe_user = "".join([c for c in username if c.isalnum() or c in ('_', '-')]) or "guest"
    return os.path.join(DATA_DIR, f"{safe_user}_contacts.txt")

def get_user_groups_path(username: str) -> str:
    safe_user = "".join([c for c in username if c.isalnum() or c in ('_', '-')]) or "guest"
    return os.path.join(DATA_DIR, f"{safe_user}_groups.txt")

def verify_user(username, password):
    if not os.path.exists(CUSTOMERS_FILE):
        return False
    hashed = hash_password(password)
    try:
        with open(CUSTOMERS_FILE, 'r', encoding='utf-8') as f:
            for line in f:
                parts = line.strip().split()
                if len(parts) >= 2:
                    u, p = parts[0], parts[1]
                    if u == username:
                        if p == hashed or p == password:
                            return True
    except Exception as e:
        print(f"[!] Error verifying user: {e}")
    return False

def register_user(username, password):
    if not username or len(username) < 3 or not password or len(password) < 4:
        return False, "Username (min 3 chars) and password (min 4 chars) required."

    if not os.path.exists(CUSTOMERS_FILE):
        open(CUSTOMERS_FILE, 'w').close()

    try:
        with open(CUSTOMERS_FILE, 'r', encoding='utf-8') as f:
            for line in f:
                parts = line.strip().split()
                if len(parts) >= 1 and parts[0] == username:
                    return False, f"Username '{username}' is already registered."
    except Exception:
        pass

    hashed = hash_password(password)
    with open(CUSTOMERS_FILE, 'a', encoding='utf-8') as f:
        f.write(f"{username} {hashed}\n")

    # Initialize user's default groups
    g_path = get_user_groups_path(username)
    if not os.path.exists(g_path):
        with open(g_path, 'w', encoding='utf-8') as f:
            f.write("4\n")
            f.write("1|Family|Immediate and extended family\n")
            f.write("2|Friends|Personal friends and buddies\n")
            f.write("3|Work|Colleagues and professional network\n")
            f.write("4|Clients|Customers and business partners\n")

    # Initialize empty contacts file
    c_path = get_user_contacts_path(username)
    if not os.path.exists(c_path):
        with open(c_path, 'w', encoding='utf-8') as f:
            f.write("0\n")

    return True, "User registered successfully."

def read_user_contacts(username):
    contacts = []
    c_path = get_user_contacts_path(username)
    if not os.path.exists(c_path):
        # Fallback for divyanshsood22 legacy
        legacy_path = os.path.join(BASE_DIR, 'contacts.txt')
        if username == 'divyanshsood22' and os.path.exists(legacy_path):
            c_path = legacy_path
        else:
            return contacts

    try:
        with open(c_path, 'r', encoding='utf-8') as f:
            lines = [line.strip() for line in f if line.strip()]
            if not lines:
                return contacts

            for line in lines[1:]:
                if '|' in line:
                    parts = line.split('|')
                    c_id = int(parts[0]) if parts[0].isdigit() else len(contacts) + 1
                    c = {
                        'id': c_id,
                        'name': parts[1] if len(parts) > 1 else '',
                        'phone': parts[2] if len(parts) > 2 else '',
                        'workPhone': parts[3] if len(parts) > 3 else '',
                        'email': parts[4] if len(parts) > 4 else '',
                        'address': parts[5] if len(parts) > 5 else '',
                        'company': parts[6] if len(parts) > 6 else '',
                        'group': parts[7] if len(parts) > 7 else 'General',
                        'isFavorite': int(parts[8]) if len(parts) > 8 and parts[8].isdigit() else 0,
                        'notes': parts[9] if len(parts) > 9 and parts[9] != '-' else '',
                        'createdAt': parts[10] if len(parts) > 10 else '2026-09-25'
                    }
                    contacts.append(c)
                else:
                    parts = line.split()
                    if len(parts) >= 2:
                        contacts.append({
                            'id': len(contacts) + 1,
                            'name': parts[0],
                            'phone': parts[1],
                            'workPhone': '',
                            'email': parts[2] if len(parts) > 2 else '',
                            'address': parts[3] if len(parts) > 3 else '',
                            'company': '',
                            'group': 'General',
                            'isFavorite': 0,
                            'notes': '',
                            'createdAt': 'Legacy Record'
                        })
    except Exception as e:
        print(f"[!] Error reading contacts for {username}: {e}")
    return contacts

def save_user_contacts(username, contacts):
    c_path = get_user_contacts_path(username)
    try:
        with open(c_path, 'w', encoding='utf-8') as f:
            f.write(f"{len(contacts)}\n")
            for c in contacts:
                notes = c.get('notes', '').strip() or '-'
                f.write(f"{c.get('id', 1)}|{c.get('name', '')}|{c.get('phone', '')}|{c.get('workPhone', '')}|"
                        f"{c.get('email', '')}|{c.get('address', '')}|{c.get('company', '')}|"
                        f"{c.get('group', 'General')}|{1 if c.get('isFavorite') else 0}|{notes}|"
                        f"{c.get('createdAt', '2026-09-25')}\n")
        return True
    except Exception as e:
        print(f"[!] Error writing contacts for {username}: {e}")
        return False

def read_user_groups(username):
    groups = []
    g_path = get_user_groups_path(username)
    if not os.path.exists(g_path):
        return [
            {'id': 1, 'name': 'Family', 'desc': 'Immediate and extended family'},
            {'id': 2, 'name': 'Friends', 'desc': 'Personal friends and buddies'},
            {'id': 3, 'name': 'Work', 'desc': 'Colleagues and professional network'},
            {'id': 4, 'name': 'Clients', 'desc': 'Customers and business partners'}
        ]
    try:
        with open(g_path, 'r', encoding='utf-8') as f:
            lines = [line.strip() for line in f if line.strip()]
            for line in lines[1:]:
                if '|' in line:
                    parts = line.split('|')
                    groups.append({
                        'id': int(parts[0]) if parts[0].isdigit() else len(groups) + 1,
                        'name': parts[1] if len(parts) > 1 else '',
                        'desc': parts[2] if len(parts) > 2 else ''
                    })
    except Exception:
        pass
    return groups

def save_user_groups(username, groups):
    g_path = get_user_groups_path(username)
    try:
        with open(g_path, 'w', encoding='utf-8') as f:
            f.write(f"{len(groups)}\n")
            for g in groups:
                desc = g.get('desc', '').strip() or 'No description'
                f.write(f"{g.get('id', 1)}|{g.get('name', '')}|{desc}\n")
        return True
    except Exception as e:
        print(f"[!] Error saving groups for {username}: {e}")
        return False

class CustomHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=GUI_DIR, **kwargs)

    def do_GET(self):
        from urllib.parse import urlparse, parse_qs
        parsed = urlparse(self.path)
        qs = parse_qs(parsed.query)

        if parsed.path == '/api/contacts':
            user = qs.get('user', ['divyanshsood22'])[0]
            contacts = read_user_contacts(user)
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(contacts).encode('utf-8'))
            return
        elif parsed.path == '/api/groups':
            user = qs.get('user', ['divyanshsood22'])[0]
            groups = read_user_groups(user)
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(groups).encode('utf-8'))
            return
        elif parsed.path == '/api/health':
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(b'{"status": "ok", "mode": "multi-user"}')
            return
        return super().do_GET()

    def do_POST(self):
        from urllib.parse import urlparse
        parsed = urlparse(self.path)
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length)

        try:
            data = json.loads(body.decode('utf-8')) if body else {}
        except Exception:
            data = {}

        if parsed.path == '/api/auth/login':
            username = data.get('username', '').strip()
            password = data.get('password', '').strip()
            if verify_user(username, password):
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'success': True, 'username': username}).encode('utf-8'))
                return
            else:
                self.send_response(401)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'success': False, 'error': 'Invalid username or password.'}).encode('utf-8'))
                return

        elif parsed.path == '/api/auth/register':
            username = data.get('username', '').strip()
            password = data.get('password', '').strip()
            ok, msg = register_user(username, password)
            status = 200 if ok else 400
            self.send_response(status)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps({'success': ok, 'message': msg, 'username': username if ok else None}).encode('utf-8'))
            return

        elif parsed.path == '/api/contacts/save':
            user = data.get('user', 'divyanshsood22').strip()
            contacts = data.get('contacts', [])
            if save_user_contacts(user, contacts):
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(b'{"success": true}')
                return

        elif parsed.path == '/api/groups/save':
            user = data.get('user', 'divyanshsood22').strip()
            groups = data.get('groups', [])
            if save_user_groups(user, groups):
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(b'{"success": true}')
                return

        self.send_response(500)
        self.end_headers()
        self.wfile.write(b'{"error": "Operation failed"}')

def run_server():
    port = DEFAULT_PORT
    server = None
    for attempt in range(10):
        try:
            socketserver.TCPServer.allow_reuse_address = True
            server = socketserver.TCPServer(("", port), CustomHandler)
            break
        except OSError as e:
            if e.errno == 98 or "already in use" in str(e).lower():
                print(f"[!] Port {port} in use, trying port {port + 1}...")
                port += 1
            else:
                raise

    if not server:
        print("[!] Fatal: Could not bind to any port between 5050 and 5060.")
        sys.exit(1)

    print("===========================================================")
    print(f" ContactHub Pro Server Active on: http://localhost:{port}")
    print(f" Storage Engine: Multi-User Isolated (in data/<user>_contacts.txt)")
    print(f" Open your browser to: http://localhost:{port}")
    print(" Press Ctrl+C to terminate the server")
    print("===========================================================")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down GUI server.")

if __name__ == '__main__':
    run_server()

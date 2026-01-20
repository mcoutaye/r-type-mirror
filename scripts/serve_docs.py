#!/usr/bin/env python3
"""
Documentation Server
Sert la documentation du moteur de jeu sur localhost
"""

import http.server
import socketserver
import os
import sys
import webbrowser
import threading
import time
import markdown
from pathlib import Path

PORT = 8080
DOCS_DIR = Path(__file__).parent.parent / "docs"

def convert_markdown_to_html(md_file, output_file):
    """Convertit un fichier markdown en HTML avec style"""
    
    with open(md_file, 'r', encoding='utf-8') as f:
        md_content = f.read()
    
    # Extensions markdown pour un meilleur rendu
    html_content = markdown.markdown(
        md_content,
        extensions=['fenced_code', 'codehilite', 'tables', 'toc']
    )
    
    # Template HTML avec style
    full_html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Game Engine Documentation</title>
    <style>
        :root {{
            --primary-color: #2c3e50;
            --secondary-color: #3498db;
            --accent-color: #e74c3c;
            --bg-color: #ecf0f1;
            --text-color: #2c3e50;
            --code-bg: #f4f4f4;
            --border-color: #bdc3c7;
        }}
        
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.6;
            color: var(--text-color);
            background: var(--bg-color);
            padding: 20px;
        }}
        
        .container {{
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        
        h1 {{
            color: var(--primary-color);
            border-bottom: 3px solid var(--secondary-color);
            padding-bottom: 10px;
            margin-bottom: 30px;
            font-size: 2.5em;
        }}
        
        h2 {{
            color: var(--secondary-color);
            margin-top: 40px;
            margin-bottom: 20px;
            font-size: 2em;
            border-left: 5px solid var(--secondary-color);
            padding-left: 15px;
        }}
        
        h3 {{
            color: var(--primary-color);
            margin-top: 30px;
            margin-bottom: 15px;
            font-size: 1.5em;
        }}
        
        h4 {{
            color: var(--text-color);
            margin-top: 20px;
            margin-bottom: 10px;
            font-size: 1.2em;
        }}
        
        p {{
            margin-bottom: 15px;
        }}
        
        code {{
            background: var(--code-bg);
            padding: 2px 6px;
            border-radius: 3px;
            font-family: 'Courier New', monospace;
            font-size: 0.9em;
            color: var(--accent-color);
        }}
        
        pre {{
            background: var(--code-bg);
            padding: 20px;
            border-radius: 5px;
            overflow-x: auto;
            margin: 20px 0;
            border-left: 4px solid var(--secondary-color);
        }}
        
        pre code {{
            background: none;
            padding: 0;
            color: var(--text-color);
        }}
        
        ul, ol {{
            margin-left: 30px;
            margin-bottom: 15px;
        }}
        
        li {{
            margin-bottom: 8px;
        }}
        
        table {{
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
        }}
        
        th, td {{
            padding: 12px;
            text-align: left;
            border: 1px solid var(--border-color);
        }}
        
        th {{
            background: var(--primary-color);
            color: white;
            font-weight: bold;
        }}
        
        tr:nth-child(even) {{
            background: #f9f9f9;
        }}
        
        a {{
            color: var(--secondary-color);
            text-decoration: none;
        }}
        
        a:hover {{
            text-decoration: underline;
        }}
        
        .toc {{
            background: #f8f9fa;
            padding: 20px;
            border-radius: 5px;
            margin-bottom: 30px;
            border: 1px solid var(--border-color);
        }}
        
        .toc h2 {{
            margin-top: 0;
            border-left: none;
            padding-left: 0;
        }}
        
        .emoji {{
            font-size: 1.2em;
        }}
        
        blockquote {{
            border-left: 4px solid var(--accent-color);
            padding-left: 20px;
            margin: 20px 0;
            color: #555;
            font-style: italic;
        }}
        
        .navigation {{
            position: fixed;
            top: 20px;
            right: 20px;
            background: white;
            padding: 15px;
            border-radius: 5px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        
        .navigation a {{
            display: block;
            margin-bottom: 10px;
            padding: 8px 12px;
            background: var(--secondary-color);
            color: white;
            border-radius: 3px;
            text-align: center;
        }}
        
        .navigation a:hover {{
            background: var(--primary-color);
            text-decoration: none;
        }}
        
        @media (max-width: 768px) {{
            .container {{
                padding: 20px;
            }}
            
            .navigation {{
                position: static;
                margin-bottom: 20px;
            }}
        }}
    </style>
</head>
<body>
    <div class="navigation">
        <a href="/">🏠 Home</a>
        <a href="/engine">🎮 Engine Doc</a>
        <a href="/doxygen/html/index.html">📚 Doxygen</a>
    </div>
    <div class="container">
        {html_content}
    </div>
</body>
</html>"""
    
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(full_html)

class DocHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(DOCS_DIR), **kwargs)
    
    def end_headers(self):
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        super().end_headers()

def open_browser():
    """Ouvre le navigateur après un court délai"""
    time.sleep(1)
    webbrowser.open(f'http://localhost:{PORT}')

def main():
    # Créer le répertoire docs s'il n'existe pas
    os.makedirs(DOCS_DIR, exist_ok=True)
    
    # Convertir la documentation markdown en HTML
    engine_doc = Path(__file__).parent.parent / "ENGINE_DOCUMENTATION.md"
    if engine_doc.exists():
        print("📝 Conversion de la documentation en HTML...")
        convert_markdown_to_html(engine_doc, DOCS_DIR / "engine.html")
    
    # Créer une page d'index
    index_html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>R-Type Engine Documentation</title>
    <style>
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 0;
        }}
        
        .container {{
            background: white;
            padding: 60px;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            text-align: center;
            max-width: 600px;
        }}
        
        h1 {{
            color: #2c3e50;
            margin-bottom: 10px;
            font-size: 2.5em;
        }}
        
        .subtitle {{
            color: #7f8c8d;
            margin-bottom: 40px;
            font-size: 1.2em;
        }}
        
        .links {{
            display: flex;
            flex-direction: column;
            gap: 20px;
        }}
        
        a {{
            display: block;
            padding: 20px 40px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            text-decoration: none;
            border-radius: 10px;
            font-size: 1.3em;
            transition: transform 0.3s, box-shadow 0.3s;
        }}
        
        a:hover {{
            transform: translateY(-5px);
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }}
        
        .emoji {{
            font-size: 1.5em;
            margin-right: 10px;
        }}
        
        .footer {{
            margin-top: 40px;
            color: #95a5a6;
            font-size: 0.9em;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🎮 R-Type Engine</h1>
        <p class="subtitle">Documentation Technique</p>
        
        <div class="links">
            <a href="/engine.html">
                <span class="emoji">📖</span>
                Documentation du Moteur
            </a>
            <a href="/doxygen/html/index.html">
                <span class="emoji">📚</span>
                Référence API (Doxygen)
            </a>
        </div>
        
        <div class="footer">
            <p>EPITECH PROJECT 2025-2026</p>
            <p>Serveur: localhost:{PORT}</p>
        </div>
    </div>
</body>
</html>"""
    
    with open(DOCS_DIR / "index.html", 'w', encoding='utf-8') as f:
        f.write(index_html)
    
    # Démarrer le serveur
    with socketserver.TCPServer(("", PORT), DocHandler) as httpd:
        print("╔════════════════════════════════════════════════╗")
        print("║       🎮 R-Type Engine Documentation          ║")
        print("╚════════════════════════════════════════════════╝")
        print(f"\n✅ Serveur démarré sur http://localhost:{PORT}")
        print(f"📁 Répertoire: {DOCS_DIR}")
        print("\n📖 Pages disponibles:")
        print(f"   → http://localhost:{PORT}/")
        print(f"   → http://localhost:{PORT}/engine.html")
        print(f"   → http://localhost:{PORT}/doxygen/index.html")
        print("\n⌨️  Appuyez sur Ctrl+C pour arrêter le serveur\n")
        
        # Ouvrir le navigateur dans un thread séparé
        threading.Thread(target=open_browser, daemon=True).start()
        
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n\n🛑 Arrêt du serveur...")
            sys.exit(0)

if __name__ == "__main__":
    main()

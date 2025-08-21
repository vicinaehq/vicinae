/**
 * GPT-5 made this, expect oddities
 *
 * QrcBuilder — build Qt .qrc files in JS (Node or browser)
 * Minimal .qrc structure:
 * <?xml version="1.0" encoding="utf-8"?>
 * <RCC version="1.0">
 *   <qresource prefix="/foo" lang="en">
 *     <file alias="logo">assets/images/logo.png</file>
 *   </qresource>
 * </RCC>
 */
class QrcBuilder {
  constructor({ version = "1.0" } = {}) {
    this.version = version;
    // Map<prefix, { lang?: string, files: Array<{ path: string, alias?: string }> }>
    this._resources = new Map();
  }

  /**
   * Ensure a resource section exists for a given prefix; optionally set lang.
   */
  addResource(prefix = "/", { lang } = {}) {
    const p = this.#normalizePrefix(prefix);
    if (!this._resources.has(p)) this._resources.set(p, { lang: undefined, files: [] });
    if (lang !== undefined) this._resources.get(p).lang = lang || undefined;
    return this;
  }

  /**
   * Add a file to a resource prefix.
   * @param {string} filePath - path as it should appear in the qrc (posix-style recommended)
   * @param {{ prefix?: string, alias?: string }} opts
   */
  addFile(filePath, { prefix = "/", alias } = {}) {
    if (!filePath || typeof filePath !== "string") {
      throw new TypeError("addFile(filePath): filePath must be a non-empty string.");
    }
    const p = this.#normalizePrefix(prefix);
    this.addResource(p); // ensure section exists
    const entry = { path: this.#toPosix(filePath), alias: alias || undefined };

    // Avoid duplicates: same path+alias under same prefix.
    const res = this._resources.get(p);
    const exists = res.files.some(f => f.path === entry.path && f.alias === entry.alias);
    if (!exists) {
      res.files.push(entry);
      // Keep deterministic order: by alias if present else by path, case-insensitive
      res.files.sort((a, b) => {
        const ak = (a.alias || a.path).toLowerCase();
        const bk = (b.alias || b.path).toLowerCase();
        return ak.localeCompare(bk);
      });
    }
    return this;
  }

  /**
   * Remove a file (match by path and optional alias) from a prefix (default "/").
   */
  removeFile(filePath, { prefix = "/", alias } = {}) {
    const p = this.#normalizePrefix(prefix);
    if (!this._resources.has(p)) return this;

    const res = this._resources.get(p);
    res.files = res.files.filter(f => !(f.path === this.#toPosix(filePath) && (alias === undefined || alias === f.alias)));

    // If empty resource section, keep it (Qt allows empty), but you can prune:
    // if (res.files.length === 0) this._resources.delete(p);
    return this;
  }

  /**
   * Set or clear the language for a prefix.
   */
  setLang(prefix = "/", lang) {
    const p = this.#normalizePrefix(prefix);
    this.addResource(p, { lang });
    return this;
  }

  /**
   * Produce the XML text of the .qrc.
   */
  toXML({ pretty = true, newline = "\n" } = {}) {
    const indent = lvl => (pretty ? "  ".repeat(lvl) : "");
    const nl = pretty ? newline : "";

    const parts = [];
    parts.push(`<?xml version="1.0" encoding="utf-8"?>${nl}`);
    parts.push(`<RCC version="${this.#esc(this.version)}">`);
    if (pretty) parts.push(nl);

    // Stable order by prefix (root "/" last to keep custom first, or vice-versa)
    const prefixes = Array.from(this._resources.keys()).sort((a, b) => a.localeCompare(b));
    for (const prefix of prefixes) {
      const { lang, files } = this._resources.get(prefix);
      const attrs = [
        `prefix="${this.#esc(prefix)}"`,
        ...(lang ? [`lang="${this.#esc(lang)}"`] : []),
      ].join(" ");
      parts.push(`${indent(1)}<qresource ${attrs}>${nl}`);

      for (const f of files) {
        const attr = f.alias ? ` alias="${this.#esc(f.alias)}"` : "";
        parts.push(`${indent(2)}<file${attr}>${this.#esc(f.path)}</file>${nl}`);
      }

      parts.push(`${indent(1)}</qresource>${nl}`);
    }

    parts.push(`</RCC>${nl}`);
    return parts.join("");
  }

  /**
   * Save to a file (Node.js only). Returns the path.
   */
  async saveTo(filePath) {
    if (!this.#isNode()) throw new Error("saveTo() is only available in Node.js.");
    const fs = await import("node:fs/promises");
    const xml = this.toXML();
    await fs.writeFile(filePath, xml, "utf8");
    return filePath;
  }

  // ---------- helpers ----------
  #normalizePrefix(prefix) {
    if (typeof prefix !== "string" || prefix.length === 0) {
      throw new TypeError("prefix must be a non-empty string");
    }
    // Must start with '/'
    return prefix.startsWith("/") ? prefix : `/${prefix}`;
  }

  #toPosix(p) {
    // Ensure forward slashes for portability in .qrc
    return p.replace(/\\/g, "/");
  }

  #esc(s) {
    return String(s)
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;")
      .replace(/'/g, "&apos;");
  }

  #isNode() {
    return typeof process !== "undefined" && process.versions && process.versions.node;
  }
}

// ---- Example usage ----
/*
const qrc = new QrcBuilder()
  .addResource("/images")
  .addFile("assets/img/logo.png", { prefix: "/images", alias: "logo" })
  .addFile("assets/img/icon.png", { prefix: "/images" })
  .addResource("/", { lang: "en" })
  .addFile("qml/Main.qml", { prefix: "/" });

console.log(qrc.toXML());

// In Node:
// await qrc.saveTo("resources.qrc");
*/

module.exports = { QrcBuilder };

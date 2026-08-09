#pragma once
#include <string_view>

inline constexpr std::string_view FIGURA_BEVE_TS = R"ts(
export type WireData = Uint8Array;

const textEncoder = new TextEncoder();
const textDecoder = new TextDecoder();

class BeveWriter {
	private buf = new Uint8Array(4096);
	private view = new DataView(this.buf.buffer);
	private len = 0;

	value(v: unknown): void {
		if (v === null || v === undefined) {
			this.u8(0x00);
			return;
		}

		switch (typeof v) {
			case "boolean":
				this.u8(v ? 0x18 : 0x08);
				return;
			case "number":
				if (Number.isSafeInteger(v)) {
					this.u8(0x69);
					this.ensure(8);
					this.view.setBigInt64(this.len, BigInt(v), true);
					this.len += 8;
				} else {
					this.u8(0x61);
					this.ensure(8);
					this.view.setFloat64(this.len, v, true);
					this.len += 8;
				}
				return;
			case "string":
				this.u8(0x02);
				this.str(v);
				return;
			case "object":
				break;
			default:
				this.u8(0x00);
				return;
		}

		if (v instanceof Uint8Array) {
			this.u8(0x14);
			this.size(v.length);
			this.ensure(v.length);
			this.buf.set(v, this.len);
			this.len += v.length;
			return;
		}

		if (Array.isArray(v)) {
			this.u8(0x05);
			this.size(v.length);
			for (const item of v) {
				this.value(item === undefined || typeof item === "function" ? null : item);
			}
			return;
		}

		const asJson = (v as { toJSON?: unknown }).toJSON;
		if (typeof asJson === "function") {
			this.value(asJson.call(v));
			return;
		}

		const obj = v as Record<string, unknown>;
		const keys = Object.keys(obj).filter(
			(k) => obj[k] !== undefined && typeof obj[k] !== "function",
		);
		this.u8(0x03);
		this.size(keys.length);
		for (const key of keys) {
			this.str(key);
			this.value(obj[key]);
		}
	}

	finish(): Uint8Array {
		return this.buf.slice(0, this.len);
	}

	private ensure(extra: number) {
		if (this.len + extra <= this.buf.length) return;
		let capacity = this.buf.length * 2;
		while (capacity < this.len + extra) capacity *= 2;
		const next = new Uint8Array(capacity);
		next.set(this.buf.subarray(0, this.len));
		this.buf = next;
		this.view = new DataView(next.buffer);
	}

	private u8(v: number) {
		this.ensure(1);
		this.buf[this.len++] = v;
	}

	private size(v: number) {
		if (v < 64) {
			this.u8(v << 2);
		} else if (v < 16384) {
			this.ensure(2);
			this.view.setUint16(this.len, (v << 2) | 1, true);
			this.len += 2;
		} else if (v < 1073741824) {
			this.ensure(4);
			this.view.setUint32(this.len, ((v << 2) | 2) >>> 0, true);
			this.len += 4;
		} else {
			this.ensure(8);
			this.view.setBigUint64(this.len, (BigInt(v) << 2n) | 3n, true);
			this.len += 8;
		}
	}

	private str(s: string) {
		const bytes = textEncoder.encode(s);
		this.size(bytes.length);
		this.ensure(bytes.length);
		this.buf.set(bytes, this.len);
		this.len += bytes.length;
	}
}

class BeveReader {
	private view: DataView;
	private pos = 0;

	constructor(private readonly buf: Uint8Array) {
		this.view = new DataView(buf.buffer, buf.byteOffset, buf.byteLength);
	}

	value(): any {
		const tag = this.buf[this.pos];

		switch (tag & 7) {
			case 0: {
				this.pos++;
				if (tag === 0x00) return null;
				return (tag & 0x10) !== 0;
			}
			case 1: {
				this.pos++;
				return this.number(tag);
			}
			case 2: {
				this.pos++;
				return this.str();
			}
			case 3: {
				this.pos++;
				if (((tag >> 3) & 3) !== 0) {
					throw new Error("BEVE: only string-keyed objects are supported");
				}
				const count = this.size();
				const obj: Record<string, any> = {};
				for (let i = 0; i < count; i++) {
					const key = this.str();
					const value = this.value();
					if (key === "__proto__") {
						Object.defineProperty(obj, key, {
							value,
							enumerable: true,
							configurable: true,
							writable: true,
						});
					} else {
						obj[key] = value;
					}
				}
				return obj;
			}
			case 4: {
				this.pos++;
				return this.typedArray(tag);
			}
			case 5: {
				this.pos++;
				const count = this.size();
				const arr = new Array(count);
				for (let i = 0; i < count; i++) arr[i] = this.value();
				return arr;
			}
			default:
				throw new Error(`BEVE: unsupported tag 0x${tag.toString(16)}`);
		}
	}

	private number(tag: number): number {
		const kind = (tag >> 3) & 3;
		const width = 1 << ((tag >> 5) & 7);
		const at = this.pos;
		this.pos += width;

		if (kind === 0) {
			if (width === 4) return this.view.getFloat32(at, true);
			if (width === 8) return this.view.getFloat64(at, true);
			throw new Error(`BEVE: unsupported float width ${width}`);
		}

		if (kind === 1) {
			if (width === 1) return this.view.getInt8(at);
			if (width === 2) return this.view.getInt16(at, true);
			if (width === 4) return this.view.getInt32(at, true);
			if (width === 8) return Number(this.view.getBigInt64(at, true));
		} else {
			if (width === 1) return this.view.getUint8(at);
			if (width === 2) return this.view.getUint16(at, true);
			if (width === 4) return this.view.getUint32(at, true);
			if (width === 8) return Number(this.view.getBigUint64(at, true));
		}

		throw new Error(`BEVE: unsupported integer width ${width}`);
	}

	private typedArray(tag: number): any {
		const kind = (tag >> 3) & 3;
		const exponent = (tag >> 5) & 7;
		const count = this.size();

		if (kind === 3) {
			if (exponent === 1) {
				const arr = new Array(count);
				for (let i = 0; i < count; i++) arr[i] = this.str();
				return arr;
			}
			const arr = new Array(count);
			for (let i = 0; i < count; i++) {
				const byte = this.buf[this.pos + (i >> 3)];
				arr[i] = (byte & (1 << (i & 7))) !== 0;
			}
			this.pos += (count + 7) >> 3;
			return arr;
		}

		const width = 1 << exponent;

		if (kind === 2 && width === 1) {
			const bytes = this.buf.slice(this.pos, this.pos + count);
			this.pos += count;
			return bytes;
		}

		const arr = new Array(count);
		for (let i = 0; i < count; i++) {
			const numberTag = (exponent << 5) | (kind << 3) | 1;
			arr[i] = this.number(numberTag);
		}
		return arr;
	}

	private size(): number {
		const config = this.buf[this.pos] & 3;
		if (config === 0) return this.buf[this.pos++] >> 2;
		if (config === 1) {
			const v = this.view.getUint16(this.pos, true) >> 2;
			this.pos += 2;
			return v;
		}
		if (config === 2) {
			const v = this.view.getUint32(this.pos, true) >>> 2;
			this.pos += 4;
			return v;
		}
		const v = this.view.getBigUint64(this.pos, true) >> 2n;
		this.pos += 8;
		return Number(v);
	}

	private str(): string {
		const n = this.size();
		const s = textDecoder.decode(this.buf.subarray(this.pos, this.pos + n));
		this.pos += n;
		return s;
	}
}

function encodeMessage(msg: JsonRpcMessage): WireData {
	const writer = new BeveWriter();
	writer.value(msg);
	return writer.finish();
}

function decodeMessage(data: WireData): JsonRpcMessage {
	return new BeveReader(data).value() as JsonRpcMessage;
}
)ts";

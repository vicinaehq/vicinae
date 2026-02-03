// Based on https://github.com/WandersonAlves/node-tail/blob/master/src/tail.ts

import events from "node:events";
import {
	accessSync,
	statSync,
	openSync,
	readSync,
	closeSync,
	createReadStream,
	watch,
	watchFile,
	unwatchFile,
	constants as fsContants,
	type FSWatcher,
	writeFileSync,
} from "node:fs";
import { dirname, join } from "node:path";
import type { EventEmitter } from "node:stream";

interface FSWatchOptions {
	interval: number;
}

export interface TailOptions {
	separator?: string | RegExp | null;
	fsWatchOptions?: FSWatchOptions;
	follow?: boolean;
	logger?: DevNull;
	useWatchFile?: boolean;
	flushAtEOF?: boolean;
	encoding?: BufferEncoding;
	fromBeginning?: boolean;
	nLines?: number;

	/**
	 * Create an empty file if it does not exist.
	 */
	forceCreate?: boolean;
}

interface QueueItem {
	start: number;
	end: number;
}

interface Cursor {
	size: number;
}

class DevNull {
	info(...args: any) { }
	error(...args: any) { }
}

export class Tail extends events.EventEmitter {
	private filename: string;
	private absPath: string;
	private separator?: string | RegExp | null;
	private fsWatchOptions: any;
	private follow: boolean;
	private logger: DevNull;
	private useWatchFile: boolean;
	private flushAtEOF: boolean;
	private encoding: BufferEncoding;
	private nLines?: number;
	private rewatchId: NodeJS.Timeout | undefined;
	private isWatching: boolean;
	private queue: QueueItem[] = [];
	// NOTE Should we rename that as it is a string instead of a Buffer?
	private buffer: string;
	private watcher: FSWatcher | undefined;
	private internalDispatcher: EventEmitter;
	private currentCursorPos: number = 0;

	constructor(filename: string, options: TailOptions = {}) {
		super();
		this.filename = filename;
		this.absPath = dirname(this.filename);
		this.separator =
			options.separator !== undefined ? options.separator : /[\r]{0,1}\n/; // null is a valid param
		this.fsWatchOptions = options.fsWatchOptions || {};
		this.follow = options.follow ?? true;
		this.logger = options.logger || new DevNull();
		this.useWatchFile = options.useWatchFile || false;
		this.flushAtEOF = options.flushAtEOF || false;
		this.encoding = options.encoding || "utf-8";
		const fromBeginning = options.fromBeginning || false;
		this.nLines = options.nLines ?? 0;

		this.logger.info(`Tail starting...`);
		this.logger.info(`filename: ${this.filename}`);
		this.logger.info(`encoding: ${this.encoding}`);

		try {
			accessSync(this.filename, fsContants.F_OK);
		} catch (err: any) {
			if (err.code === "ENOENT") {
				if (!options.forceCreate) throw err;
				writeFileSync(this.filename, "");
			}
		}

		this.buffer = "";
		this.internalDispatcher = new events.EventEmitter();
		this.isWatching = false;

		// this.internalDispatcher.on('next',this.readBlock);
		this.internalDispatcher.on("next", () => {
			this.readBlock();
		});

		let cursor = 0;

		this.logger.info(`fromBeginning: ${fromBeginning}`);
		if (fromBeginning) {
			cursor = 0;
		} else if (this.nLines <= 0) {
			cursor = 0;
		} else if (this.nLines !== undefined) {
			cursor = this.getPositionAtNthLine(this.nLines);
		} else {
			cursor = this.latestPosition();
		}

		if (cursor === undefined) throw new Error("Tail can't initialize.");

		const flush = fromBeginning || this.nLines !== undefined;
		try {
			this.watch(cursor, flush);
		} catch (err) {
			this.logger.error(`watch for ${this.filename} failed: ${err}`);
			this.emit("error", `watch for ${this.filename} failed: ${err}`);
		}
	}

	/**
	 * Grabs the index of the last line of text in the format /.*(\n)?/.
	 * Returns null if a full line can not be found.
	 * @param {string} text
	 * @returns {number | null}
	 */
	private getIndexOfLastLine(text: string): number | null {
		/**
		 * Helper function get the last match as string
		 * @param {string} haystack
		 * @param {string | RegExp} needle
		 * @returns {string | undefined}
		 */
		const getLastMatch = (
			haystack: string,
			needle: string | RegExp | null,
		): string | undefined => {
			// NOTE `as string` was used to cast the needle to string, but it can be null as well. Just making TS compiler happy
			const matches = haystack.match(needle as string);
			if (matches === null) {
				return;
			}

			return matches[matches.length - 1];
		};
		// NOTE `as string` was used to cast the needle to string, but it can be null as well. Just making TS compiler happy
		const endSep = getLastMatch(text, this.separator as string);

		if (!endSep) return null;

		const endSepIndex = text.lastIndexOf(endSep);
		let lastLine = "";

		if (text.endsWith(endSep)) {
			// If the text ends with a separator, look back further to find the next
			// separator to complete the line

			const trimmed = text.substring(0, endSepIndex);
			// NOTE `as string` was used to cast the needle to string, but it can be null as well. Just making TS compiler happy
			const startSep = getLastMatch(trimmed, this.separator as string);

			// If there isn't another separator, the line isn't complete so
			// so return null to get more data

			if (!startSep) {
				return null;
			}

			const startSepIndex = trimmed.lastIndexOf(startSep);

			// Exclude the starting separator, include the ending separator

			lastLine = text.substring(
				startSepIndex + startSep.length,
				endSepIndex + endSep.length,
			);
		} else {
			// If the text does not end with a separator, grab everything after
			// the last separator
			lastLine = text.substring(endSepIndex + endSep.length);
		}

		return text.lastIndexOf(lastLine);
	}

	/**
	 * Returns the position of the start of the `nLines`th line from the bottom.
	 * Returns 0 if `nLines` is greater than the total number of lines in the file.
	 * @param {number} nLines
	 * @returns {number}
	 */
	private getPositionAtNthLine(nLines: number): number {
		const { size } = statSync(this.filename);

		if (size === 0) {
			return 0;
		}

		const fd = openSync(this.filename, "r");
		// Start from the end of the file and work backwards in specific chunks
		let currentReadPosition = size;
		const chunkSizeBytes = Math.min(1024, size);
		const lineBytes = [] as number[];

		let remaining = "";

		while (lineBytes.length < nLines) {
			// Shift the current read position backward to the amount we're about to read
			currentReadPosition -= chunkSizeBytes;

			// If negative, we've reached the beginning of the file and we should stop and return 0, starting the
			// stream at the beginning.
			if (currentReadPosition < 0) {
				return 0;
			}

			// Read a chunk of the file and prepend it to the working buffer
			const buffer = Buffer.alloc(chunkSizeBytes);
			const bytesRead = readSync(
				fd,
				buffer,
				0, // position in buffer to write to
				chunkSizeBytes, // number of bytes to read
				currentReadPosition, // position in file to read from
			);

			// .subarray returns Uint8Array in node versions < 16.x and Buffer
			// in versions >= 16.x. To support both, allocate a new buffer with
			// Buffer.from which accepts both types
			const readArray = buffer.subarray(0, bytesRead);
			remaining = Buffer.from(readArray).toString(this.encoding) + remaining;

			let index = this.getIndexOfLastLine(remaining);

			while (index !== null && lineBytes.length < nLines) {
				const line = remaining.substring(index);

				lineBytes.push(Buffer.byteLength(line));
				remaining = remaining.substring(0, index);

				index = this.getIndexOfLastLine(remaining);
			}
		}

		closeSync(fd);

		return size - lineBytes.reduce((acc, cur) => acc + cur, 0);
	}

	private latestPosition() {
		try {
			return statSync(this.filename).size;
		} catch (err) {
			this.logger.error(`size check for ${this.filename} failed: ${err}`);
			this.emit("error", `size check for ${this.filename} failed: ${err}`);
			throw err;
		}
	}

	private readBlock() {
		if (this.queue.length >= 1) {
			const block = this.queue[0];
			if (block!.end > block.start) {
				const stream = createReadStream(this.filename, {
					start: block.start,
					end: block.end - 1,
					encoding: this.encoding,
				});
				stream.on("error", (error) => {
					this.logger.error(`Tail error: ${error}`);
					this.emit("error", error);
				});
				stream.on("end", () => {
					this.queue.shift();
					if (this.queue.length > 0) {
						this.internalDispatcher.emit("next");
					}
					if (this.flushAtEOF && this.buffer.length > 0) {
						this.emit("line", this.buffer);
						this.buffer = "";
					}
				});
				stream.on("data", (d) => {
					if (this.separator === null) {
						this.emit("line", d);
					} else {
						this.buffer += d;
						// NOTE `as string` was used to cast the needle to string, but it can be null as well. Just making TS compiler happy
						let parts = this.buffer.split(this.separator as string);
						// NOTE Since parts.pop could return undefined, i'm returning a empty string when that happens
						this.buffer = parts.pop() ?? "";
						for (const chunk of parts) {
							this.emit("line", chunk);
						}
					}
				});
			}
		}
	}

	private change() {
		const p = this.latestPosition();
		if (p < this.currentCursorPos) {
			//scenario where text is not appended but it's actually a w+
			this.currentCursorPos = p;
		} else if (p > this.currentCursorPos) {
			this.queue.push({ start: this.currentCursorPos, end: p });
			this.currentCursorPos = p;
			if (this.queue.length === 1) {
				this.internalDispatcher.emit("next");
			}
		}
	}

	watch(startingCursor: number, flush?: boolean) {
		if (this.isWatching) return;
		this.logger.info(`filesystem.watch present? ${watch !== undefined}`);
		this.logger.info(`useWatchFile: ${this.useWatchFile}`);

		this.isWatching = true;
		this.currentCursorPos = startingCursor;
		//force a file flush is either fromBegining or nLines flags were passed.
		if (flush) this.change();

		if (!this.useWatchFile) {
			this.logger.info(`watch strategy: watch`);
			this.watcher = watch(
				this.filename,
				this.fsWatchOptions,
				(e, filename) => {
					// NOTE Filename here is a `Buffer`, how it's used as a string?
					// NOTE Test if filename.toString changes the behavior
					if (filename) this.watchEvent(e, filename.toString());
				},
			);
		} else {
			this.logger.info(`watch strategy: watchFile`);
			watchFile(this.filename, this.fsWatchOptions, (curr, prev) => {
				this.watchFileEvent(curr, prev);
			});
		}
	}

	private rename(filename: string) {
		//TODO
		//MacOS sometimes throws a rename event for no reason.
		//Different platforms might behave differently.
		//see https://nodejs.org/api/fs.html#fs_fs_watch_filename_options_listener
		//filename might not be present.
		//https://nodejs.org/api/fs.html#fs_filename_argument
		//Better solution would be check inode but it will require a timeout and
		// a sync file read.
		if (filename === undefined || filename !== this.filename) {
			this.unwatch();
			if (this.follow) {
				this.filename = join(this.absPath, filename);
				this.rewatchId = setTimeout(() => {
					try {
						this.watch(this.currentCursorPos);
					} catch (ex) {
						this.logger.error(
							`'rename' event for ${this.filename}. File not available anymore.`,
						);
						this.emit("error", ex);
					}
				}, 1000);
			} else {
				this.logger.error(
					`'rename' event for ${this.filename}. File not available anymore.`,
				);
				this.emit(
					"error",
					`'rename' event for ${this.filename}. File not available anymore.`,
				);
			}
		} else {
			// this.logger.info("rename event but same filename")
		}
	}

	private watchEvent(evtName: "change" | "rename", evtFilename: string) {
		try {
			if (evtName === "change") {
				this.change();
			} else if (evtName === "rename") {
				this.rename(evtFilename);
			}
		} catch (err) {
			this.logger.error(`watchEvent for ${this.filename} failed: ${err}`);
			this.emit("error", `watchEvent for ${this.filename} failed: ${err}`);
		}
	}

	private watchFileEvent(curr: Cursor, prev: Cursor) {
		if (curr.size > prev.size) {
			this.currentCursorPos = curr.size; //Update this.currentCursorPos so that a consumer can determine if entire file has been handled
			this.queue.push({ start: prev.size, end: curr.size });
			if (this.queue.length === 1) {
				this.internalDispatcher.emit("next");
			}
		}
	}

	unwatch() {
		if (this.watcher) {
			this.watcher.close();
		} else {
			unwatchFile(this.filename);
		}
		if (this.rewatchId) {
			clearTimeout(this.rewatchId);
			this.rewatchId = undefined;
		}
		this.isWatching = false;
		this.queue = []; // TODO: is this correct behaviour?
		if (this.logger) {
			this.logger.info(`Unwatch ${this.filename}`);
		}
	}
}

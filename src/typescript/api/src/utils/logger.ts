import { red, magenta, blue, green, reset, gray } from "./yoctocolors";

export class Logger {
	prefixes = {
		error: `${red("error")}${reset()}`,
		event: `${magenta("event")}${reset()}`,
		info: `${blue("info")}${reset()}`,
		ready: `${green("ready")}${reset()}`,
	};

	logError(message: string) {
		console.log(`${this.prefixes.error.padEnd(15)} - ${message}`);
	}

	logEvent(message: string) {
		console.log(`${this.prefixes.event.padEnd(15)} - ${message}`);
	}

	logInfo(message: string) {
		console.log(`${this.prefixes.info.padEnd(15)} - ${message}`);
	}

	logReady(message: string) {
		console.log(`${this.prefixes.ready.padEnd(15)} - ${message}`);
	}

	logExtensionOut(s: string) {
		this.logTimestamp(s);
	}

	logExtensionError(s: string) {
		this.logTimestamp(`${red(s)}${reset()}`);
	}

	logTimestamp(s: string) {
		const ts = new Date().toJSON();
		const lines = s.split("\n");

		for (let i = 0; i !== lines.length; ++i) {
			const line = lines[i];

			if (i === lines.length - 1 && line.length === 0) continue;

			console.log(`${gray(ts.padEnd(20))}${reset()} - ${line}`);
		}
	}
}

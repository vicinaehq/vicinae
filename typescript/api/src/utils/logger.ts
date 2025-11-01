import chalk from "chalk";

export class Logger {
	prefixes = {
		error: `${chalk.red("error")}${chalk.reset()}`,
		event: `${chalk.magenta("event")}${chalk.reset()}`,
		info: `${chalk.blue("info")}${chalk.reset()}`,
		ready: `${chalk.green("ready")}${chalk.reset()}`,
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

	logTimestamp(s: string) {
		const ts = new Date().toJSON();
		const lines = s.split("\n");

		for (let i = 0; i !== lines.length; ++i) {
			const line = lines[i];

			if (i === lines.length - 1 && line.length === 0) continue;

			console.log(`${chalk.gray(ts.padEnd(20))}${chalk.reset()} - ${line}`);
		}
	}
}

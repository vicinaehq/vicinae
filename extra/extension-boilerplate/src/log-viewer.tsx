import { Action, ActionPanel, Icon, List } from "@vicinae/api";
import {
	bold,
	dim,
	red,
	green,
	yellow,
	cyan,
	magenta,
	magentaBright,
} from "./yoctocolors.js";

type LogLevel = "DEBUG" | "INFO" | "WARN" | "ERROR" | "FATAL";

type LogEntry = {
	timestamp: string;
	level: LogLevel;
	service: string;
	message: string;
	detail?: string;
};

const levelColor: Record<LogLevel, (s: string) => string> = {
	DEBUG: magentaBright,
	INFO: green,
	WARN: yellow,
	ERROR: red,
	FATAL: (s: string) => bold(red(s)),
};

const levelIcon: Record<LogLevel, Icon> = {
	DEBUG: Icon.Bug,
	INFO: Icon.Info,
	WARN: Icon.Warning,
	ERROR: Icon.XmarkCircle,
	FATAL: Icon.XmarkCircle,
};

function formatLogLine(entry: LogEntry): string {
	const lvl = levelColor[entry.level];
	return (
		dim(entry.timestamp) +
		" " +
		lvl(entry.level.padEnd(5)) +
		" " +
		cyan(`[${entry.service}]`) +
		" " +
		entry.message
	);
}

function formatDetail(entry: LogEntry): string {
	const lvl = levelColor[entry.level];

	let md = `<pre>${lvl(entry.level)} ${dim("—")} ${bold(entry.service)}\n`;
	md += `${dim(entry.timestamp)}\n\n`;
	md += `${entry.message}</pre>\n\n`;

	if (entry.detail) {
		md += `\`\`\`\n${entry.detail}\n\`\`\``;
	}

	return md;
}

const logs: LogEntry[] = [
	{
		timestamp: "2026-03-18 09:00:01.123",
		level: "INFO",
		service: "Gateway",
		message: "HTTP server listening on :8080",
	},
	{
		timestamp: "2026-03-18 09:00:01.456",
		level: "INFO",
		service: "DatabasePool",
		message: "Connection pool initialized " + green("(12 connections)"),
	},
	{
		timestamp: "2026-03-18 09:00:02.001",
		level: "DEBUG",
		service: "AuthService",
		message: "JWT signing key rotated, new kid=" + dim("ab3f...c8e1"),
	},
	{
		timestamp: "2026-03-18 09:00:05.334",
		level: "INFO",
		service: "CacheWarmer",
		message:
			"Cache primed: " +
			green("2,431") +
			" entries loaded in " +
			cyan("340ms"),
	},
	{
		timestamp: "2026-03-18 09:01:12.891",
		level: "WARN",
		service: "RateLimiter",
		message:
			"Client " +
			yellow("192.168.1.42") +
			" approaching rate limit (" +
			yellow("487/500") +
			" req/min)",
	},
	{
		timestamp: "2026-03-18 09:01:45.002",
		level: "DEBUG",
		service: "MailService",
		message: "Queued " + magenta("3") + " emails for batch delivery",
	},
	{
		timestamp: "2026-03-18 09:02:33.100",
		level: "ERROR",
		service: "PaymentGateway",
		message:
			"Stripe webhook verification " + red("failed") + " — signature mismatch",
		detail:
			"Error: Webhook signature verification failed\n" +
			"  at verifySignature (lib/stripe.ts:42)\n" +
			"  at handleWebhook (routes/webhooks.ts:18)\n" +
			"Expected sig: whsec_abc123...\n" +
			"Received sig: whsec_000000...",
	},
	{
		timestamp: "2026-03-18 09:02:33.105",
		level: "WARN",
		service: "PaymentGateway",
		message: "Retrying webhook in " + yellow("30s") + " (attempt 1/3)",
	},
	{
		timestamp: "2026-03-18 09:03:01.222",
		level: "INFO",
		service: "SchedulerService",
		message:
			"Cron job " +
			cyan("cleanup-stale-sessions") +
			" completed: removed " +
			green("148") +
			" sessions",
	},
	{
		timestamp: "2026-03-18 09:03:15.800",
		level: "DEBUG",
		service: "SearchIndex",
		message:
			"Reindex batch " +
			dim("#47") +
			": " +
			green("1,024") +
			" documents in " +
			cyan("120ms"),
	},
	{
		timestamp: "2026-03-18 09:04:02.010",
		level: "ERROR",
		service: "DatabasePool",
		message:
			"Connection " + red("timed out") + " after 30s — pool exhausted",
		detail:
			"ActiveConnections: 12/12\n" +
			"WaitingRequests: 34\n" +
			"LongestWait: 28.4s\n" +
			"Query: SELECT * FROM orders WHERE status = 'pending'\n" +
			"  AND created_at > NOW() - INTERVAL '1 hour'",
	},
	{
		timestamp: "2026-03-18 09:04:02.015",
		level: "FATAL",
		service: "HealthCheck",
		message:
			"Service " +
			bold(red("UNHEALTHY")) +
			" — database pool exhausted, triggering restart",
	},
	{
		timestamp: "2026-03-18 09:04:10.300",
		level: "INFO",
		service: "Gateway",
		message:
			"Graceful shutdown initiated, draining " +
			yellow("12") +
			" in-flight requests",
	},
	{
		timestamp: "2026-03-18 09:04:12.500",
		level: "INFO",
		service: "Gateway",
		message: "All connections drained. Server " + green("stopped") + " cleanly.",
	},
];

export default function LogViewer() {
	return (
		<List isShowingDetail searchBarPlaceholder="Filter logs...">
			<List.Section title="Application Logs — 2026-03-18">
				{logs.map((entry, i) => (
					<List.Item
						key={i.toString()}
						title={formatLogLine(entry)}
						icon={levelIcon[entry.level]}
						keywords={[entry.level, entry.service]}
						detail={
							<List.Item.Detail markdown={formatDetail(entry)} />
						}
						actions={
							<ActionPanel>
								<Action.CopyToClipboard
									title="Copy Log Line"
									content={formatLogLine(entry)}
								/>
								{entry.detail && (
									<Action.CopyToClipboard
										title="Copy Stack Trace"
										content={entry.detail}
									/>
								)}
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}

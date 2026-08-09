const values = ["development", "production"] as const;

export type EnvironmentType = (typeof values)[number];

export type WorkerManagerMessage = {
	channel: "manager";
	data: Uint8Array;
};

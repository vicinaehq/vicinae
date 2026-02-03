export type Result<T, E = undefined> = { unwrap: () => T } & (
	| { ok: true; value: T }
	| { ok: false; error: E | undefined }
);

export const Ok = <T>(data: T): Result<T, never> => {
	return { ok: true, value: data, unwrap: () => data };
};

export const Err = <E>(error?: E): Result<never, E> => {
	return {
		ok: false,
		error,
		unwrap: () => {
			throw error;
		},
	};
};

import { useEffect, useState } from "react";
import { Application, getApplications } from "../utils";

export const useApplications = (): [Application[], boolean, Error | null] => {
	const [isLoading, setIsLoading] = useState(true);
	const [error, setError] = useState<Error | null>(null);
	const [apps, setApps] = useState<Application[]>([]);

	useEffect(() => {
		setIsLoading(true);
		getApplications()
			.then(setApps)
			.catch(setError)
			.finally(() => setIsLoading(false));
	}, []);

	return [apps, isLoading, error];
};

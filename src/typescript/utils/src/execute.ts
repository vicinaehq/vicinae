import { execFile } from "node:child_process";
import { promisify } from "node:util";
import { useQuery } from "./query.js";

const executeAsync = promisify(execFile);

async function execute(command: string, args: readonly string[]) {
  try {
    const { stdout } = await executeAsync(command, args);
    return stdout;
  } catch (error) {
    const { stderr = "" } = error as Error & { stderr?: string };
    throw new Error(stderr.trim() || "Command failed");
  }
}

export function useExec(command: string, args: readonly string[] = []) {
  return useQuery<string>({
    queryKey: ["execute", command, args],
    queryFn: () => execute(command, args),
  });
}

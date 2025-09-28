import { PathLike } from "fs";
import { rm } from "fs/promises";
import { bus } from "./bus";
import { WindowManagement } from "./window-management";
import { Application } from "./proto/application";

// Linux systems usually do not have a trash, but maybe we should support one...
export const trash = async (path: PathLike | PathLike[]): Promise<void> => {
  const targets = Array.isArray(path) ? path : [path];
  const promises = targets.map((p) => rm(p, { recursive: true }));

  await Promise.all(promises);
};

export const open = async (target: string, app?: Application | string) => {
  let appId: string | undefined;

  if (app) {
    if (typeof app === "string") {
      appId = app;
    } else {
      appId = app.id;
    }
  }

  await bus.turboRequest("app.open", {
    target,
    appId,
  });
};

export const getFrontmostApplication = async (): Promise<Application> => {
  const { application } = await WindowManagement.getActiveWindow();

  if (!application) {
	throw new Error(`Could not get frontmost application`);
  }

  return application;
};

export const getApplications = async (
  target?: string
): Promise<Application[]> => {
  const res = await bus.turboRequest("app.list", { target });

  return res.unwrap().apps;
};

export const getDefaultApplication = async (
  path: string,
): Promise<Application> => {
   const res = await bus.turboRequest('app.getDefault', { target: path });
   const app = res.unwrap().app

   if (!app) throw new Error(`No default application for target ${path}`);

   return app;
};

export const showInFileBrowser = async (path: PathLike): Promise<void> => {
	const fileBrowser = await getDefaultApplication("inode/directory"); // FIXME: we may want something more robust
	await open(path.toString(), fileBrowser);
};

export { Application } from "./proto/application";

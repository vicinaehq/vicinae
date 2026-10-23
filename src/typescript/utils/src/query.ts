import { Cache } from "@vicinae/api";
import { experimental_createQueryPersister } from "@tanstack/query-persist-client-core";
import {
  QueryClient,
  type DefaultError,
  type QueryKey,
  type UseQueryOptions,
  useQuery as useTanStackQuery,
} from "@tanstack/react-query";

const queryClient = new QueryClient();
const cache = new Cache({ namespace: "tanstack-query" });
const { persisterFn } = experimental_createQueryPersister({
  storage: {
    getItem: cache.get,
    setItem: cache.set,
    removeItem: (key) => {
      cache.remove(key);
    },
  },
});

export function useQuery<
  TQueryFnData = unknown,
  TError = DefaultError,
  TData = TQueryFnData,
  TQueryKey extends QueryKey = QueryKey,
>(options: UseQueryOptions<TQueryFnData, TError, TData, TQueryKey>) {
  return useTanStackQuery(options, queryClient);
}

export function usePersistedQuery<
  TQueryFnData = unknown,
  TError = DefaultError,
  TData = TQueryFnData,
  TQueryKey extends QueryKey = QueryKey,
>(options: UseQueryOptions<TQueryFnData, TError, TData, TQueryKey>) {
  return useTanStackQuery({ ...options, persister: persisterFn }, queryClient);
}

/**
 * @license React
 * react-reconciler-reflection.production.js
 *
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

"use strict";
function formatProdErrorMessage(code) {
  var url = "https://react.dev/errors/" + code;
  if (1 < arguments.length) {
    url += "?args[]=" + encodeURIComponent(arguments[1]);
    for (var i = 2; i < arguments.length; i++)
      url += "&args[]=" + encodeURIComponent(arguments[i]);
  }
  return (
    "Minified React error #" +
    code +
    "; visit " +
    url +
    " for the full message or use the non-minified dev environment for full errors and additional helpful warnings."
  );
}
function getNearestMountedFiber(fiber) {
  var node = fiber,
    nearestMounted = fiber;
  if (fiber.alternate) for (; node.return; ) node = node.return;
  else {
    fiber = node;
    do
      (node = fiber),
        0 !== (node.flags & 4098) && (nearestMounted = node.return),
        (fiber = node.return);
    while (fiber);
  }
  return 3 === node.tag ? nearestMounted : null;
}
function assertIsMounted(fiber) {
  if (getNearestMountedFiber(fiber) !== fiber)
    throw Error(formatProdErrorMessage(188));
}
function findCurrentFiberUsingSlowPath(fiber) {
  var alternate = fiber.alternate;
  if (!alternate) {
    alternate = getNearestMountedFiber(fiber);
    if (null === alternate) throw Error(formatProdErrorMessage(188));
    return alternate !== fiber ? null : fiber;
  }
  for (var a = fiber, b = alternate; ; ) {
    var parentA = a.return;
    if (null === parentA) break;
    var parentB = parentA.alternate;
    if (null === parentB) {
      b = parentA.return;
      if (null !== b) {
        a = b;
        continue;
      }
      break;
    }
    if (parentA.child === parentB.child) {
      for (parentB = parentA.child; parentB; ) {
        if (parentB === a) return assertIsMounted(parentA), fiber;
        if (parentB === b) return assertIsMounted(parentA), alternate;
        parentB = parentB.sibling;
      }
      throw Error(formatProdErrorMessage(188));
    }
    if (a.return !== b.return) (a = parentA), (b = parentB);
    else {
      for (var didFindChild = !1, child$0 = parentA.child; child$0; ) {
        if (child$0 === a) {
          didFindChild = !0;
          a = parentA;
          b = parentB;
          break;
        }
        if (child$0 === b) {
          didFindChild = !0;
          b = parentA;
          a = parentB;
          break;
        }
        child$0 = child$0.sibling;
      }
      if (!didFindChild) {
        for (child$0 = parentB.child; child$0; ) {
          if (child$0 === a) {
            didFindChild = !0;
            a = parentB;
            b = parentA;
            break;
          }
          if (child$0 === b) {
            didFindChild = !0;
            b = parentB;
            a = parentA;
            break;
          }
          child$0 = child$0.sibling;
        }
        if (!didFindChild) throw Error(formatProdErrorMessage(189));
      }
    }
    if (a.alternate !== b) throw Error(formatProdErrorMessage(190));
  }
  if (3 !== a.tag) throw Error(formatProdErrorMessage(188));
  return a.stateNode.current === a ? fiber : alternate;
}
function findCurrentHostFiberImpl(node) {
  var tag = node.tag;
  if (5 === tag || 26 === tag || 27 === tag || 6 === tag) return node;
  for (node = node.child; null !== node; ) {
    tag = findCurrentHostFiberImpl(node);
    if (null !== tag) return tag;
    node = node.sibling;
  }
  return null;
}
function findCurrentHostFiberWithNoPortalsImpl(node) {
  var tag = node.tag;
  if (5 === tag || 26 === tag || 27 === tag || 6 === tag) return node;
  for (node = node.child; null !== node; ) {
    if (
      4 !== node.tag &&
      ((tag = findCurrentHostFiberWithNoPortalsImpl(node)), null !== tag)
    )
      return tag;
    node = node.sibling;
  }
  return null;
}
function traverseFragmentInstanceChildren(child, fn, a, b, c) {
  for (; null !== child; ) {
    if (5 === child.tag) {
      if (fn(child.stateNode, a, b, c)) break;
    } else
      (22 === child.tag && null !== child.memoizedState) ||
        traverseFragmentInstanceChildren(child.child, fn, a, b, c);
    child = child.sibling;
  }
}
exports.doesFiberContain = function (parentFiber, childFiber) {
  for (
    var parentFiberAlternate = parentFiber.alternate;
    null !== childFiber;

  ) {
    if (childFiber === parentFiber || childFiber === parentFiberAlternate)
      return !0;
    childFiber = childFiber.return;
  }
  return !1;
};
exports.findCurrentFiberUsingSlowPath = findCurrentFiberUsingSlowPath;
exports.findCurrentHostFiber = function (parent) {
  parent = findCurrentFiberUsingSlowPath(parent);
  return null !== parent ? findCurrentHostFiberImpl(parent) : null;
};
exports.findCurrentHostFiberWithNoPortals = function (parent) {
  parent = findCurrentFiberUsingSlowPath(parent);
  return null !== parent ? findCurrentHostFiberWithNoPortalsImpl(parent) : null;
};
exports.getContainerFromFiber = function (fiber) {
  return 3 === fiber.tag ? fiber.stateNode.containerInfo : null;
};
exports.getNearestMountedFiber = getNearestMountedFiber;
exports.getSuspenseInstanceFromFiber = function (fiber) {
  if (13 === fiber.tag) {
    var suspenseState = fiber.memoizedState;
    null === suspenseState &&
      ((fiber = fiber.alternate),
      null !== fiber && (suspenseState = fiber.memoizedState));
    if (null !== suspenseState) return suspenseState.dehydrated;
  }
  return null;
};
exports.isFiberSuspenseAndTimedOut = function (fiber) {
  var memoizedState = fiber.memoizedState;
  return (
    13 === fiber.tag &&
    null !== memoizedState &&
    null === memoizedState.dehydrated
  );
};
exports.traverseFragmentInstance = function (fragmentFiber, fn, a, b, c) {
  traverseFragmentInstanceChildren(fragmentFiber.child, fn, a, b, c);
};

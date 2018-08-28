// Copyright (C) 2018 The Xaya developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef XAYAGAME_GAME_HPP
#define XAYAGAME_GAME_HPP

#include "mainloop.hpp"
#include "storage.hpp"
#include "uint256.hpp"
#include "zmqsubscriber.hpp"

#include "rpc-stubs/xayarpcclient.h"

#include <json/json.h>
#include <jsonrpccpp/client.h>

#include <memory>
#include <mutex>
#include <string>

namespace xaya
{

/**
 * The interface for actual games.  Implementing classes define the rules
 * of an actual game so that it can be plugged into libxayagame to form
 * a complete game engine.
 */
class GameLogic
{

public:

  virtual ~GameLogic () = default;

  /**
   * Returns the initial state (as well as the associated block height
   * and block hash in big-endian hex) for the game on the given chain
   * ("main", "test" or "regtest").
   */
  virtual void GetInitialState (const std::string& chain,
                                unsigned& height, std::string& hashHex,
                                GameStateData& state) = 0;

};

/**
 * The main class implementing a game on the Xaya platform.  It handles the
 * ZMQ and RPC communication with the Xaya daemon as well as the RPC interface
 * of the game itself.
 *
 * To implement a game, create a subclass that overrides the pure virtual
 * methods with the actual game logic and then instantiate and Run() it
 * from the binary's main().
 */
class Game : private internal::ZmqListener
{

private:

  /** This game's game ID.  */
  const std::string gameId;

  /**
   * Mutex guarding internal state.  This is necessary at least in theory since
   * changes might be made from the ZMQ listener on the ZMQ subscriber's
   * worker thread in addition to the main thread.
   */
  mutable std::mutex mut;

  /** The chain type (main, test, regtest) to which the game is connected.  */
  std::string chain;

  /** The JSON-RPC client connection to the Xaya daemon.  */
  std::unique_ptr<XayaRpcClient> rpcClient;

  /** The ZMQ subscriber.  */
  internal::ZmqSubscriber zmq;

  /** Storage system in use.  */
  StorageInterface* storage = nullptr;

  /** The game rules in use.  */
  GameLogic* rules = nullptr;

  /** The main loop.  */
  internal::MainLoop mainLoop;

  /**
   * The JSON-RPC version to use for talking to Xaya Core.  The actual daemon
   * needs V1, but for the unit test (where the server is mocked and set up
   * based on jsonrpccpp), we want V2.
   */
  static jsonrpc::clientVersion_t rpcClientVersion;

  void BlockAttach (const std::string& id, const Json::Value& data,
                    bool seqMismatch) override;
  void BlockDetach (const std::string& id, const Json::Value& data,
                    bool seqMismatch) override;

  friend class GameTests;

public:

  explicit Game (const std::string& id);

  Game () = delete;
  Game (const Game&) = delete;
  void operator= (const Game&) = delete;

  /**
   * Sets up the RPC client based on the given connector.
   */
  void ConnectRpcClient (jsonrpc::IClientConnector& conn);

  /**
   * Returns the chain (network type, "main", "test" or "regtest") of the
   * connected Xaya daemon.  This can be used to set up the storage database
   * correctly, for instance.  Must not be called before ConnectRpcClient.
   */
  const std::string& GetChain () const;

  /**
   * Sets the storage interface to use.  This must be called before starting
   * the main loop, and may not be called while it is running.
   */
  void SetStorage (StorageInterface* s);

  /**
   * Sets the game rules to use.  This must be called before starting
   * the main loop, and may not be called while it is running.
   */
  void SetGameLogic (GameLogic* gl);

  /**
   * Sets the ZMQ endpoint that will be used to connect to the ZMQ interface
   * of the Xaya daemon.  Must not be called anymore after StartZmq() or
   * Run() have been called.
   */
  void
  SetZmqEndpoint (const std::string& addr)
  {
    zmq.SetEndpoint (addr);
  }

  /**
   * Detects the ZMQ endpoint by calling getzmqnotifications on the Xaya
   * daemon.  Returns false if pubgameblocks is not enabled.
   */
  bool DetectZmqEndpoint ();

  /**
   * Adds this game's ID to the tracked games of the core daemon.
   */
  void TrackGame ();

  /**
   * Removes this game's ID from the tracked games of the core daemon.
   */
  void UntrackGame ();

  /**
   * Starts the ZMQ subscriber in a new thread.  Must only be called after
   * the ZMQ endpoint has been configured, and must not be called when
   * ZMQ is already running.
   */
  void
  StartZmq ()
  {
    zmq.Start ();
  }

  /**
   * Stops the ZMQ subscriber.  Must only be called if it is currently running.
   */
  void
  StopZmq ()
  {
    zmq.Stop ();
  }

  /**
   * Runs the main event loop for the Game.  This starts all configured
   * subsystems (ZMQ, RPC server) and blocks the calling thread until
   * a stop of the server is requested.  Then those subsystems are stopped
   * again and the method returns.
   */
  void Run ();

};

} // namespace xaya

#endif // XAYAGAME_GAME_HPP

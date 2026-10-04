function on_start(a,b,c)
    server.broadcast("[freeroam] Resource started")
end

function on_command(player_id, command)
    if command == "/heal" then
        player.set_health(player_id, 100.0)
        server.message_player(player_id, "Health restored.")
        return true
    end
    if command == "/spawncar" then
        -- Example coordinates only; a real gamemode should use the player's current position.
        local id = vehicle.create(411, 0.0, 0.0, 5.0, 0.0)
        server.message_player(player_id, "Vehicle created: " .. tostring(id))
        return true
    end
    return false
end

function on_tick(now_ms)
    -- Keep frequent work small. Prefer timers/events for heavier gamemode logic.
end

function on_stop(a,b,c)
end
